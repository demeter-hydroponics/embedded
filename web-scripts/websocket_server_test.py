import asyncio
import websockets

# note: if running in WSL, may need to do netsh interface stuff to forward

# add root of project to python path
import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(os.path.join(os.path.dirname(__file__), "..", "generated", "python"))

from generated.python import common_pb2
from generated.python.metrics import node_stats_pb2
from generated.python.pump import mixing_stats_pb2


# construct a fake common pb2 message to get the size of the serialized message
def get_common_pb2_size():
    common_pb2_message = common_pb2.MessageHeader()
    common_pb2_message.channel = common_pb2.MessageChannels.NODE_STATS
    common_pb2_message.timestamp = 1234567890
    common_pb2_message.length = 0
    return common_pb2_message.ByteSize()


COMMON_PB2_SIZE_BYTES = get_common_pb2_size()


def print_messages(frame):
    # get the common_pb2 message
    frame_size_bytes = len(frame)
    pointer = 0
    while pointer < frame_size_bytes:
        header = common_pb2.MessageHeader()
        header.ParseFromString(frame[pointer : pointer + COMMON_PB2_SIZE_BYTES])
        pointer += COMMON_PB2_SIZE_BYTES
        print(header)

        if header.channel == common_pb2.MessageChannels.NODE_STATS:
            node_stats = node_stats_pb2.NodeStats()
            node_stats.ParseFromString(frame[pointer : pointer + header.length])
            pointer += header.length
            print(node_stats)
        elif header.channel == common_pb2.MessageChannels.MIXING_STATS:
            mixing_stats = mixing_stats_pb2.MixingTankStats()
            mixing_stats.ParseFromString(frame[pointer : pointer + header.length])
            pointer += header.length
            print(mixing_stats)


# Handler for each client connection
async def handle_client(websocket):
    client_address = websocket.remote_address[0]
    print(f"New client connected: {client_address}")
    await websocket.send("Welcome to the WebSocket server!")

    try:
        async for message in websocket:
            # print(f"Message received from {client_address}: {message}")
            print_messages(message)
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
