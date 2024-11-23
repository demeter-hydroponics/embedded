import asyncio
import websockets


async def websocket_client():
    uri = "ws://172.19.49.193:9001"

    async with websockets.connect(uri) as websocket:
        print(f"Connected to WebSocket server at {uri}")

        async def receive_messages():
            while True:
                try:
                    response = await websocket.recv()
                    print(f"Server: {response}")
                except websockets.ConnectionClosed:
                    print("Connection closed by server.")
                    break

        # Run the receiver in the background
        asyncio.create_task(receive_messages())

        # Type and send messages to the server
        while True:
            try:
                message = input("You: ")
                await websocket.send(message)
            except (KeyboardInterrupt, EOFError):
                print("\nClosing connection...")
                break

        # Close the connection
        await websocket.close()
        print("Connection closed.")


# Run the WebSocket client
asyncio.run(websocket_client())
