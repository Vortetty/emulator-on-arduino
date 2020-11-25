byte clamp(byte x, byte min, byte max){
    return (byte)min(max(x, min), max);
}


// Radian management, pulled from https://forum.arduino.cc/index.php?topic=98147.msg736263#msg736263

byte radToDeg(byte rads){
    return (rads * 4068) / 71;
}

byte degToRad(byte degs){
    return (degs * 71) / 4068;
}