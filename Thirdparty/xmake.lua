target("CSerialPort")
    set_kind("static")
    add_files(
        "CSerialPort/src/SerialPort.cpp",
        "CSerialPort/src/SerialPortBase.cpp",
        "CSerialPort/src/SerialPortInfo.cpp",
        "CSerialPort/src/SerialPortInfoBase.cpp",
        "CSerialPort/src/SerialPortInfoUnixBase.cpp",
        "CSerialPort/src/SerialPortUnixBase.cpp"
    )

target("crossguid")
    set_kind("static")
    add_files("crossguid/src/guid.cpp")
