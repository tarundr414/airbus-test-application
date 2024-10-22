#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "satellite.h"



// Function to perform system health check (Command 101)
Telemetry_101 perform_health_check() {
    Telemetry_101 tm;
    tm.status_code = 201; // Status code for health check
    tm.data[0] = rand() % 101; // Simulate CPU usage
    tm.data[1] = rand() % 101; // Simulate Memory usage
    tm.data[2] = 100 - (rand() % 101); // Simulate Battery level

    // Print the health check information
    printf("Health Check: Status Code: %d, CPU Usage: %.2f%%, Memory Usage: %.2f%%, Battery Level: %.2f%%\n",
           tm.status_code, tm.data[0], tm.data[1], tm.data[2]);
    return tm;
}

// Function to perform orbital adjustment and send orbital data report (Command 102)
void perform_orbital_adjustment(Telecommand_102 tc, int new_socket, char *response) {
    // Capture the new velocities in X, Y, and Z
    float new_velocity_x = tc.params[0];
    float new_velocity_y = tc.params[1];
    float new_velocity_z = tc.params[2]; // Capture the new velocity in Z

    // Initialize Telemetry for orbital adjustment report
    Telemetry_102 tm;
    tm.status_code = 202; // Status code for orbital data report
    tm.data[0] = 500.0; // Initial altitude in km
    tm.data[1] = new_velocity_x; // Initial Velocity X
    tm.data[2] = new_velocity_y; // Initial Velocity Y
    tm.data[3] = new_velocity_z; // Initial Velocity Z

    // Send the initial orbital data report
    snprintf(response, 1024, "%d %.2f %.2f %.2f %.2f\n",
             tm.status_code, tm.data[0], tm.data[1], tm.data[2], tm.data[3]);

    // Print the data being sent for the initial orbital adjustment report
    printf("Sending Orbital Data Report: Status Code: %d, Altitude: %.2f km, Velocity X: %.2f m/s, Velocity Y: %.2f m/s, Velocity Z: %.2f m/s\n",
           tm.status_code, tm.data[0], tm.data[1], tm.data[2], tm.data[3]);

    send(new_socket, response, strlen(response), 0);

    // Send orbital data report every 5 seconds for 5 times
    for (int i = 1; i <= ORBITAL_DATA_REPORT_COUNT; i++) {
        sleep(ORBITAL_DATA_REPORT_INTERVAL);
        
        // Update altitude and velocity for demonstration
        tm.data[0] = 500.0 + (i * 10); // Increment altitude
        tm.data[1] = new_velocity_x + (i * 0.5); // Increment velocity X
        tm.data[2] = new_velocity_y + (i * 0.5); // Increment velocity Y
        tm.data[3] = new_velocity_z + (i * 0.5); // Increment velocity Z

        // Prepare and send the report including all velocity components
        snprintf(response, 1024, "%d %.2f %.2f %.2f %.2f\n",
                 tm.status_code, tm.data[0], tm.data[1], tm.data[2], tm.data[3]);

        // Print the data being sent for subsequent orbital data reports
        printf("Sending Orbital Data Report %d: Status Code: %d, Altitude: %.2f km, Velocity X: %.2f m/s, Velocity Y: %.2f m/s, Velocity Z: %.2f m/s\n",
               i, tm.status_code, tm.data[0], tm.data[1], tm.data[2], tm.data[3]);

        send(new_socket, response, strlen(response), 0);
    }
}

// Function to manage payload operation (Command 103)
void manage_payload_operation(Telecommand_103 tc, int new_socket, char *response) {
    Telemetry_103 tm;
    tm.status_code = 203; // Status code for payload data report
    tm.payload_id = tc.payload_id; // Payload ID
    tm.operational_status = tc.operational_code; // Operational status

    // Check if the payload is being activated
    if (tm.operational_status == 1) { // Active
        printf("Activating Payload ID %d\n", tm.payload_id);
        // Simulate sending payload data every 10 seconds for 90 seconds
        for (int i = 0; i < PAYLOAD_DATA_REPORT_COUNT; i++) {
            sleep(PAYLOAD_DATA_REPORT_INTERVAL);
            tm.data[0] = (float)(rand() % 100); // Example measurement 1
            tm.data[1] = (float)(rand() % 100); // Example measurement 2
            tm.data[2] = (float)(rand() % 100); // Example measurement 3

            // Send telemetry back to the client
            snprintf(response, 1024, "%d %d %d %.2f %.2f %.2f\n",
                     tm.status_code, tm.payload_id, tm.operational_status,
                     tm.data[0], tm.data[1], tm.data[2]);

            // Print the data being sent for payload operations
            printf("Sending Payload Data Report: Status Code: %d, Payload ID: %d, Operational Status: %d, Measurements: %.2f, %.2f, %.2f\n",
                   tm.status_code, tm.payload_id, tm.operational_status, 
                   tm.data[0], tm.data[1], tm.data[2]);

            send(new_socket, response, strlen(response), 0);
        }
    } else {
        printf("Deactivating Payload ID %d\n", tm.payload_id);
        // No further action required for deactivation
        tm.data[0] = 0.0; // No data if inactive
        tm.data[1] = 0.0;
        tm.data[2] = 0.0;
        // Send telemetry back to the client
        snprintf(response, 1024, "%d %d %d %.2f %.2f %.2f\n",
                 tm.status_code, tm.payload_id, tm.operational_status,
                 tm.data[0], tm.data[1], tm.data[2]);
        send(new_socket, response, strlen(response), 0);
    }
}

// Main function
int main() {
    srand(time(0)); // Seed random number generator

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    // Declare response buffer
    char response[1024];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Satellite is running... Waiting for commands.\n");

    while (1) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                                 (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Read the telecommand from the client
        read(new_socket, buffer, 1024);
        
        Telecommand_102 tc;
        if (sscanf(buffer, "%d %f %f %f", &tc.command_id, 
                   &tc.params[0], &tc.params[1], &tc.params[2]) < 1) {
            printf("Invalid command format\n");
            close(new_socket);
            continue;
        }

        // Process command based on command ID
        switch (tc.command_id) {
            case 101: {
                // Report health status 10 times, every 2 seconds
                for (int i = 0; i < REPORT_COUNT; i++) {
                    Telemetry_101 tm = perform_health_check();
                    // Send telemetry back to the client
                    snprintf(response, sizeof(response), "%d %.2f %.2f %.2f\n",
                             tm.status_code, tm.data[0], tm.data[1], tm.data[2]);
                    send(new_socket, response, strlen(response), 0);
                    sleep(REPORT_INTERVAL); // Delay for 2 seconds
                }
                break;
            }
            case 102:
                perform_orbital_adjustment(tc, new_socket, response);
                break;
            case 103: {
                Telecommand_103 tc_103;
                sscanf(buffer, "%d %d %d", &tc_103.command_id, &tc_103.operational_code, &tc_103.payload_id);
                manage_payload_operation(tc_103, new_socket, response);
                break;
            }
            default:
                printf("Unknown Command ID\n");
                break;
        }

        close(new_socket);  // Close connection after processing command
    }

    return 0;
}
