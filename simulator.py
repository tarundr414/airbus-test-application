import socket
import logging

# Constants for Ground Station
SERVER_IP = "127.0.0.1"  # Assuming the satellite program runs on the same machine
SERVER_PORT = 8080
BUFFER_SIZE = 1024

# Configure logging
logging.basicConfig(
                    level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s')

def send_command(command_id, params):
    """Sends a command to the satellite and logs telemetry data."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connect to the server
        try:
            sock.connect((SERVER_IP, SERVER_PORT))
        except ConnectionRefusedError:
            logging.error("Connection failed. Make sure the satellite is running.")
            print("Connection failed. Make sure the satellite is running.")
            return
        
        # Prepare the command message
        command_message = f"{command_id} {params[0]} {params[1]} {params[2]}"
        sock.sendall(command_message.encode())
        logging.info(f"Command sent: {command_message}")
        # print(f"Command sent: {command_message}")

        # Receive telemetry data
        while True:
            response = sock.recv(BUFFER_SIZE)
            if not response:
                break  # No more data from the server
            telemetry_data = response.decode().strip()
            log_telemetry(telemetry_data)

def log_telemetry(telemetry_data):
    """Logs telemetry data based on the status code received."""
    try:
        data_parts = telemetry_data.split()
        status_code = data_parts[0]

        if status_code == "201":  # Health Check response
            if len(data_parts) < 4:
                logging.warning("Received insufficient data for health check.")
                return
            cpu_usage = data_parts[1]
            memory_usage = data_parts[2]
            battery_level = data_parts[3]
            log_message = (
                f"TM_{status_code} System Health Status received "
                f"CPU Usage: {cpu_usage}, "
                f"Memory Usage: {memory_usage}, "
                f"Battery Level: {battery_level}"
            )
        
        elif status_code == "202":  # Orbital Data Report
            if len(data_parts) < 5:
                logging.warning("Received insufficient data for orbital data report.")
                return
            current_altitude = data_parts[1]
            velocity_x = data_parts[2]
            velocity_y = data_parts[3]
            velocity_z = data_parts[4]
            log_message = (
                f"TM_{status_code} Orbital Data Report received "
                f"Current Altitude: {current_altitude} km, "
                f"Velocity Vector: [{velocity_x} m/s, {velocity_y} m/s, {velocity_z} m/s]"
            )
        
        elif status_code == "203":  # Payload Data Report
            if len(data_parts) < 6:
                logging.warning("Received insufficient data for payload data report.")
                return
            payload_id = data_parts[1]
            operational_status = data_parts[2]
            measurement_1 = data_parts[3]
            measurement_2 = data_parts[4]
            measurement_3 = data_parts[5]
            log_message = (
                f"TM_{status_code} Payload Data received "
                f"Payload ID: {payload_id}, "
                f"Operational Status: {operational_status}, "
                f"Measurements: [{measurement_1}, {measurement_2}, {measurement_3}]"
            )
        
        else:
            logging.warning(f"Unknown status code received: {status_code}")
            return

        logging.info(log_message)
        # print(log_message)  # Optional: Print to console for real-time feedback

    except Exception as e:
        logging.error(f"Error processing telemetry data: {e}")


def main():
    while True:
        try:
            # Get user input for command ID
            command_id = int(input("Enter command ID (101 for health check, 102 for orbital adjustment, 103 for payload operation, 0 to exit): "))
            if command_id == 0:
                logging.info("Exiting ground station.")
                print("Exiting ground station.")
                break
            
            params = [0.0, 0.0, 0.0]

            # Get parameters based on command ID
            if command_id == 102:  # Orbital adjustment
                params[0] = float(input("Enter DeltaVx: "))
                params[1] = float(input("Enter DeltaVy: "))
                params[2] = float(input("Enter DeltaVz: "))
                logging.info(f"Orbital adjustment parameters: DeltaVx={params[0]}, DeltaVy={params[1]}, DeltaVz={params[2]}")
            elif command_id == 103:  # Manage payload
                params[0] = float(input("Enter operation code (1 for activate, 0 for deactivate): "))
                params[1] = float(input("Enter payload ID: "))
                params[2] = 0.0  # Set placeholder for unused parameter
                logging.info(f"Payload operation parameters: OperationCode={params[0]}, PayloadID={params[1]}")

            send_command(command_id, params)

        except ValueError:
            logging.warning("Invalid input received.")
            print("Invalid input. Please enter a number.")

if __name__ == "__main__":
    logging.info("Ground station started.")
    main()
