1. Run python SimpleHTTPserver 
2. Modify function GetHttp in PlayerControl.cpp to python server address and filename
3. Open "NewMap" map in unreal
4. Compile & play level
5. TCP socket is open in local ip and port 10000
6. Send "make name_here" using ncat or similar to local.ip PORT
7. Send "move name_here X Y Z" to move the object
