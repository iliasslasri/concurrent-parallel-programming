import asyncio

async def async_func1():
    return 1

async def async_func2():
    return 2

async def main():
    print("Start")
    task = asyncio.create_task(async_func1())
    await task
    await asyncio.gather(async_func2(), task)
    await asyncio.sleep(1)
    print("Done")

asyncio.run(main())