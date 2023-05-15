void IniciarMotor() {
  digitalWrite(STEP, HIGH);
  delay(Velocidad);
  digitalWrite(STEP, LOW);
  ////// Encoder //////
  Encoder = digitalRead(Encoder_pin);
  if (Encoder == LOW && EstadoEncoder == 1) {
    EstadoEncoder = 0;
  }
  if (Encoder == HIGH && EstadoEncoder == 0) {
    Pulsos++;
    EstadoEncoder = 1;
  }
  if (Pulsos == PulsosRotacion) {
    FinalTime = millis();
    RPMs = 60000 / (FinalTime - InitialTime);
    Pulsos = 0;
    InitialTime = FinalTime;
    Serial.print("RPM = ");
    Serial.println(RPMs);
  }
  
}
