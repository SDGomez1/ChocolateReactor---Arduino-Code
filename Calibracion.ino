void Calibracion() {

  double Escala;
  double PromMedicion;
  unsigned short Peso_conocido;

  Serial.print("Cual es su peso conocido: ");  // Solicita al usuario ingresar el peso conocido
  // Espera a que se ingrese el peso conocido
  delay(3000);
  Peso_conocido = Serial.parseInt();  // Obtiene el valor del peso conocido ingresado
  Serial.println(Peso_conocido);      // Imprime el valor del peso conocido ingresado
  delay(1000);
  balanza.tare(20);     // Establece la tara en la balanza
  balanza.set_scale();  // Establece la escala en la balanza
  Serial.println("~ CALIBRACIÓN DE LA BALANZA ~");
  Serial.println("Coloque un peso conocido ");
  delay(2500);

  for (int i = 3; i >= 0; i--) {  // Realiza cuatro mediciones analógicas para obtener el promedio
    Serial.print(" ... ");
    Serial.print(i);
    PromMedicion = balanza.get_value(20);  //20 mediciones  //Obtiene el promedio de las mediciones analogas según valor ingresado
  }
  Serial.println(" ");
  Serial.println("Retire el peso ");
  delay(250);

  for (int i = 3; i >= 0; i--) {  // Realiza una cuenta regresiva de 4 segundos
    Serial.print(" ... ");
    Serial.print(i);
    delay(1000);
  }
  Serial.println(" ");

  Escala = PromMedicion / Peso_conocido;  // Relación entre el promedio de las mediciones analogas con el peso conocido en gramos
  balanza.set_scale(Escala);              // Ajusta la escala correspondiente

  if (balanza.get_scale() > -1250 || balanza.get_scale() < -1300)
    Serial.println("La calibracion ha fallado, Por favor vuelva a intentarlo");
  else
    Serial.println("Balanza calibrada con exito");

  Serial.println(balanza.get_scale());
  Serial.println(balanza.get_offset());
}
