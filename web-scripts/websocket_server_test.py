import asyncio
import websockets


# Handler for each client connection
async def handle_client(websocket):
    client_address = websocket.remote_address[0]
    print(f"New client connected: {client_address}")
    await websocket.send("Welcome to the WebSocket server!")

    try:
        async for message in websocket:
            print(f"Message received from {client_address}: {message}")
            # Echo the message back to the client
            await websocket.send(f"Echo: {message}")
    except websockets.ConnectionClosed:
        print(f"Client {client_address} disconnected")


# Main function to start the server
async def main():
    server = await websockets.serve(handle_client, "0.0.0.0", 9001)
    print("WebSocket server is running on ws://0.0.0.0:9001")
    await server.wait_closed()


# Entry point
if __name__ == "__main__":
    asyncio.run(main())
