#define TM_MAX_PARAMS_101 3
#define TC_MAX_PARAMS_102 3
#define TM_MAX_PARAMS_102 4 
#define TM_MAX_PARAMS_103 3
#define PORT 8080
#define REPORT_COUNT 10
#define REPORT_INTERVAL 2 // seconds
#define ORBITAL_DATA_REPORT_COUNT 5
#define ORBITAL_DATA_REPORT_INTERVAL 5 // seconds
#define PAYLOAD_DATA_REPORT_COUNT 9  // Number of payload reports (10 seconds each for 90 seconds)
#define PAYLOAD_DATA_REPORT_INTERVAL 10 // seconds

// Telecommand Structure for Command 101 (Health Check)
typedef struct {
    int command_id; // Command ID for health check
} Telecommand_101;

// Telecommand Structure for Command 102 (Orbital Adjustment)
typedef struct {
    int command_id;
    float params[TC_MAX_PARAMS_102]; // Current Altitude, DeltaVx, DeltaVy, DeltaVz for command 102
} Telecommand_102;

// Telecommand Structure for Command 103 (Payload Management)
typedef struct {
    int command_id;
    int operational_code;
    int payload_id;
} Telecommand_103;

// Telemetry Structure for Command 101 (Health Check)
typedef struct {
    int status_code;
    float data[TM_MAX_PARAMS_101]; // Data values: CPU usage, Memory usage, Battery level
} Telemetry_101;

// Telemetry Structure for Command 102 (Orbital Data)
typedef struct {
    int status_code;
    float data[TM_MAX_PARAMS_102]; // Data values: Altitude, Velocity X, Velocity Y, Velocity Z
} Telemetry_102;

// Telemetry Structure for Command 103 (Payload Data)
typedef struct {
    int status_code;
    int payload_id;
    int operational_status;
    float data[TM_MAX_PARAMS_103]; // Payload data values
} Telemetry_103;