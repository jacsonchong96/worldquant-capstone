import asyncio
import websockets
import json
import os
import csv
import time

OUTPUT_DIRECTORY = "data"
OUT_FILE = os.path.join(OUTPUT_DIRECTORY, "l2_data.csv")

# just a global list for now. We have hardcoded it initially. It will turn into a dynamic universe by volume later 
PRODUCTS = ["BTC-USD", "ETH-USD", "SOL-USD", "ADA-USD"]

async def record_l2():
    os.makedirs(OUTPUT_DIRECTORY, exist_ok=True)
    # opening the CSV file in append mode
    with open(OUT_FILE, "a", newline="") as f_opener:
        writer = csv.writer(f_opener)
        if f_opener.tell() == 0:
            writer.writerow(["ts_recv_ns", "ws_idx", "raw"]) 
        uri = "wss://advanced-trade-ws.coinbase.com"
        async with websockets.connect(uri, max_size=None) as webskt:
            sub_msg = {
                "type": "subscribe",
                "channel": "level2",
                "product_ids": PRODUCTS,
            }
            await webskt.send(json.dumps(sub_msg))
            # print(f"[webskt#0] Subscribed to {PRODUCTS}")

            async for msg in webskt:
                ts_ns = time.time_ns()
                # rmving both newlines and commas before writing and flushing to disk
                writer.writerow([ts_ns, 0, msg.replace("\n", " ").replace(",", ";")])
                f_opener.flush()  # flushing immediately to disk
                # print(f"[WS#0] recv_ns={ts_ns}")

if __name__ == "__main__":
    try:
        asyncio.run(record_l2())
    except KeyboardInterrupt:
        print("Stopped by user")
