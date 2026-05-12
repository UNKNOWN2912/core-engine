import os.path
import time

filePath = "swapchain.comp"

modifiedTime = os.path.getmtime(filePath)

while 1:
    currentTime = os.path.getmtime(filePath)

    if(modifiedTime != currentTime):
        modifiedTime = currentTime
        os.system("./compileLoop.bash")
    
    time.sleep(1)
