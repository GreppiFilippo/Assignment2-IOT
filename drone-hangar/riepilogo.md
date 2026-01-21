# Riepilogo Sistema di Messaggistica - Drone Hangar

## 🎯 Architettura Generale

Il sistema utilizza un'architettura **task-based** con comunicazione centralizzata attraverso il **Context**.

```
┌─────────────┐
│   Serial    │
└──────┬──────┘
       │
       v
┌─────────────────────────────────────────────────────────────┐
│                        MsgTask                               │
│  ┌──────────────────┐              ┌──────────────────┐     │
│  │  INPUT (JSON)    │              │  OUTPUT (JSON)   │     │
│  │  Parsing         │              │  Building        │     │
│  └────────┬─────────┘              └────────▲─────────┘     │
└───────────┼─────────────────────────────────┼───────────────┘
            │                                  │
            v                                  │
    ┌───────────────────────────────────────────────────┐
    │                   Context                         │
    │  ┌──────────────────────┐  ┌──────────────────┐  │
    │  │  Message Queue (IN)  │  │  JsonDoc (OUT)   │  │
    │  │  - TTL 5s            │  │  - Dynamic       │  │
    │  │  - FIFO con ricerca  │  │  - ArduinoJson   │  │
    │  └──────────────────────┘  └──────────────────┘  │
    └───────────────────────────────────────────────────┘
            ▲                                  ▲
            │                                  │
            v                                  │
    ┌──────────────────────────────────────────────────┐
    │  DroneTask, AlarmTask, LCDTask, etc.            │
    │  - Consumano messaggi (IN)                       │
    │  - Scrivono campi JSON (OUT)                     │
    └──────────────────────────────────────────────────┘
```

---

## 📥 INPUT: Ricezione Comandi

### **Formato**
I comandi arrivano in formato JSON dalla seriale:
```json
{"command": "OPEN DOOR"}
```

### **Flusso**
1. **SerialEvent** → legge byte dalla seriale e costruisce messaggio
2. **MsgService** → mantiene 1 messaggio temporaneo
3. **MsgTask** → consuma messaggio e:
   - Fa **parsing JSON** con ArduinoJson
   - Estrae campo `"command"`
   - Inserisce nella **message queue** del Context con **timestamp**
4. **Tasks** → cercano e consumano i comandi che servono

### **Message Queue - Caratteristiche**

```cpp
struct TimestampedMsg {
    String content;
    unsigned long timestamp;
    bool valid;
};
```

- **Capacità**: 10 messaggi (`MSG_QUEUE_SIZE`)
- **TTL**: 5 secondi (`MSG_TIMEOUT_MS`)
- **Ricerca**: In **tutta la coda**, non solo FIFO head
- **Pulizia**: Automatica ad ogni tick di MsgTask

### **API per le Task**

```cpp
// Controllare se esiste un messaggio
OpenPattern pattern;  // Pattern che matcha "OPEN DOOR"
if (pContext->hasMessage(pattern)) {
    // Il messaggio esiste
}

// Consumare un messaggio
if (pContext->consumeMessage(pattern)) {
    // Messaggio trovato e consumato
}
```

### **Esempio Pattern**
```cpp
class OpenPattern : public Pattern {
public:
    bool match(const Msg& m) override {
        return m.getContent().equals("OPEN DOOR");
    }
};
```

---

## 📤 OUTPUT: Invio Stato

### **Formato**
Lo stato viene inviato periodicamente in formato JSON:
```json
{"droneState":"TAKING_OFF","distance":25.5,"doorOpen":true,"alarm":false}
```

### **Flusso**
1. **Tasks** → scrivono campi nel JsonDocument del Context
2. **MsgTask** → ogni 500ms (`JSON_UPDATE_PERIOD_MS`):
   - Costruisce JSON da tutti i campi attivi
   - Invia via seriale
   - **Cancella** tutti i campi (reset)

### **API per le Task**

```cpp
// String
pContext->setJsonField("droneState", "TAKING_OFF");

// Float
pContext->setJsonField("distance", 25.5);
pContext->setJsonField("temperature", 28.3);

// Int
pContext->setJsonField("doorAngle", 90);

// Boolean
pContext->setJsonField("alarm", false);

// Rimuovere campo
pContext->removeJsonField("temperature");
```

### **Comportamento**
- ✅ **Sovrascrittura automatica**: Stesso campo viene aggiornato
- ✅ **Invio periodico**: Ogni 500ms
- ✅ **Reset automatico**: Dopo invio, JSON viene svuotato
- ✅ **Type-safe**: ArduinoJson gestisce tipi automaticamente

---

## 🔄 MsgTask - Il Cuore del Sistema

### **Responsabilità**
1. **Pulizia messaggi scaduti** (> 5s)
2. **Ricezione e parsing comandi JSON**
3. **Invio periodico stato JSON**

### **Tick Flow**
```cpp
void MsgTask::tick() {
    // 1. Pulisci messaggi scaduti (TTL)
    cleanExpiredMessages();
    
    // 2. Ricevi nuovi comandi (INPUT)
    if (msgAvailable) {
        parseJSON → extractCommand → addToQueue
    }
    
    // 3. Invia stato (OUTPUT) ogni 500ms
    if (elapsed >= 500ms) {
        buildJSON → send → clearJSON
    }
}
```

---

## 📦 Context - Stato Condiviso

### **Message Queue (INPUT)**
```cpp
bool addMessage(const String& msg);
bool hasMessage(Pattern& pattern);
bool consumeMessage(Pattern& pattern);
int cleanExpiredMessages();
```

### **JSON Output (OUTPUT)**
```cpp
void setJsonField(const String& key, const String& value);
void setJsonField(const String& key, float value);
void setJsonField(const String& key, int value);
void setJsonField(const String& key, bool value);
void removeJsonField(const String& key);
String buildJSON();
void clearJsonFields();
```

### **Altri Stati**
- Door control (open/close requests)
- Alarm flags (alarm, pre-alarm)
- Sensor data (distance, temperature, PIR)
- LCD message
- Blinking LED state

---

## ⚙️ Configurazione

### **File: config.hpp**

```cpp
// Message system
#define MSG_QUEUE_SIZE 10           // Capacità coda
#define MSG_TIMEOUT_MS 5000         // TTL messaggi (5s)
#define JSON_UPDATE_PERIOD_MS 500   // Periodo invio JSON (500ms)

// Commands
#define OPEN_CMD "OPEN DOOR"        // Comando apertura porta
```

---

## 🎯 Vantaggi dell'Architettura

### ✅ **Separazione delle responsabilità**
- MsgTask → gestisce I/O seriale
- Context → mantiene stato condiviso
- Tasks → logica applicativa

### ✅ **Nessun accoppiamento**
- Le task non conoscono MsgTask
- Le task non conoscono Serial
- Comunicazione solo via Context

### ✅ **Robustezza**
- TTL previene memory leak
- Ricerca in tutta la coda previene race condition
- JSON parsing gestisce errori
- ArduinoJson è type-safe

### ✅ **Efficienza**
- Invio batch periodico (non ad ogni change)
- Messaggi scaduti puliti automaticamente
- Zero-copy con ArduinoJson

---

## 📝 Esempio Completo di Uso

### **DroneTask - Consumare comando**
```cpp
void DroneTask::tick() {
    switch (state) {
        case REST:
            OpenPattern pattern;
            if (pContext->consumeMessage(pattern)) {
                // Ricevuto comando di apertura
                setState(TAKING_OFF);
            }
            break;
            
        case TAKING_OFF:
            // Aggiorna stato per invio remoto
            pContext->setJsonField("droneState", "TAKING_OFF");
            pContext->setJsonField("doorOpen", true);
            
            if (!droneIn()) {
                setState(OPERATING);
            }
            break;
    }
}
```

### **Comunicazione Remoto ↔ Arduino**

**Remoto invia:**
```json
{"command": "OPEN DOOR"}
```

**Arduino risponde (ogni 500ms):**
```json
{"droneState":"TAKING_OFF","distance":45.3,"doorOpen":true,"alarm":false}
```

---

## 🛠️ Dipendenze

### **PlatformIO**
```ini
lib_deps = 
    paulstoffregen/TimerOne@^1.2
    marcoschwartz/LiquidCrystal_I2C@^1.1.4
    bblanchon/ArduinoJson@^7.2.0
```

### **ArduinoJson**
- Versione: 7.2.0
- Usato per: parsing comandi IN + building stato OUT
- Zero-copy, type-safe, ottimizzato per embedded

---

## 📚 File Principali

```
src/
├── kernel/
│   ├── MsgService.hpp/cpp      # Gestione seriale (1 msg buffer)
│   └── Logger.hpp/cpp          # Logging
├── model/
│   └── Context.hpp/cpp         # Stato condiviso + message queue + JSON
├── task/
│   └── MSGTask.hpp/cpp         # I/O seriale bidirezionale
└── config.hpp                  # Configurazioni e costanti
```
