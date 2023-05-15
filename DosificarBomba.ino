void DosificarB() {
  float pesoLiquido = balanza.get_units(1);  // Entrega el peso del liquido actualmente medido en gramos
  float Progreso = (pesoLiquido * 100) / setPoint;
  if (setPoint >= pesoLiquido) {  // Verifica si el volumen deseado ha sido alcanzado
    digitalWrite(PinBomba2, HIGH);
    if (Progreso >= 20 && Progreso <= 20.3) {
      Serial.println("Progreso: 20%");

    } else if (Progreso >= 40 && Progreso <= 40.3) {
      Serial.println("Progreso: 40%");

    } else if (Progreso >= 60 && Progreso <= 60.3) {
      Serial.println("Progreso: 60%");

    } else if (Progreso >= 80 && Progreso <= 80.3) {
      Serial.println("Progreso: 80%");
    }
  } else {
    digitalWrite(PinBomba2, LOW);
    Dosificando = 0;
  }
}
