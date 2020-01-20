/*
 * Double layer perceptron example
 * Sigmoid activation function
 * Backpropagation
 */
//#include <math.h>
 
const int inQ = 4; //2;
const int hiddQ = 8; //5;
const int outQ = 7; //2;
float out[outQ]; //выход сети
const float LearningRate = 0.7; //скорость обучения
const float InitialWeightMax = 0.5; //максимум диапазона случайных значений стартовых весов
const float Momentum = 0.9;
const float Success = 0.005;
const int PatternCount = 10;

float hNeuron[hiddQ][2][inQ+1];
float oNeuron[outQ][2][hiddQ+1];
float changeHiddNeuron[hiddQ][inQ+1];
float changeOutNeuron[outQ][hiddQ+1];
/*
//сет
const byte Input[PatternCount][inQ] = {
  { 1, 1, 1, 1, 1, 1, 0 },  // 0
  { 0, 1, 1, 0, 0, 0, 0 },  // 1
  { 1, 1, 0, 1, 1, 0, 1 },  // 2
  { 1, 1, 1, 1, 0, 0, 1 },  // 3
  { 0, 1, 1, 0, 0, 1, 1 },  // 4
  { 1, 0, 1, 1, 0, 1, 1 },  // 5
  { 0, 0, 1, 1, 1, 1, 1 },  // 6
  { 1, 1, 1, 0, 0, 0, 0 },  // 7 
  { 1, 1, 1, 1, 1, 1, 1 },  // 8
  { 1, 1, 1, 0, 0, 1, 1 }   // 9
}; 

//правильные значения
const byte Target[PatternCount][outQ] = {
  { 0, 0, 0, 0 },  
  { 0, 0, 0, 1 }, 
  { 0, 0, 1, 0 }, 
  { 0, 0, 1, 1 }, 
  { 0, 1, 0, 0 }, 
  { 0, 1, 0, 1 }, 
  { 0, 1, 1, 0 }, 
  { 0, 1, 1, 1 }, 
  { 1, 0, 0, 0 }, 
  { 1, 0, 0, 1 } 
};
*/
//сет
const byte Target[PatternCount][outQ] = {
  { 1, 1, 1, 1, 1, 1, 0 },  // 0
  { 0, 1, 1, 0, 0, 0, 0 },  // 1
  { 1, 1, 0, 1, 1, 0, 1 },  // 2
  { 1, 1, 1, 1, 0, 0, 1 },  // 3
  { 0, 1, 1, 0, 0, 1, 1 },  // 4
  { 1, 0, 1, 1, 0, 1, 1 },  // 5
  { 0, 0, 1, 1, 1, 1, 1 },  // 6
  { 1, 1, 1, 0, 0, 0, 0 },  // 7 
  { 1, 1, 1, 1, 1, 1, 1 },  // 8
  { 1, 1, 1, 0, 0, 1, 1 }   // 9
}; 

//правильные значения
const byte Input[PatternCount][inQ] = {
  { 0, 0, 0, 0 },  
  { 0, 0, 0, 1 }, 
  { 0, 0, 1, 0 }, 
  { 0, 0, 1, 1 }, 
  { 0, 1, 0, 0 }, 
  { 0, 1, 0, 1 }, 
  { 0, 1, 1, 0 }, 
  { 0, 1, 1, 1 }, 
  { 1, 0, 0, 0 }, 
  { 1, 0, 0, 1 } 
};

int i, j, p, q, r;
int randomizedIndex[PatternCount];
long epoch;
 
void setup() {
  Serial.begin(9600); // устанавливаем последовательное соединение
  delay(1000);
  randomSeed(analogRead(0));
  for(int i=0; i<hiddQ; i++){
    for(int j=0; j<=inQ; j++){
      changeHiddNeuron[i][j] = 0.0;
      hNeuron[i][0][j] = 2.0 * ((float(random(100))/100) - 0.5) * InitialWeightMax;
    }
  }
  for(int i=0; i<outQ; i++){
    for(int j=0; j<=hiddQ; j++){
      changeOutNeuron[i][j] = 0.0;
      oNeuron[i][0][j] = 2.0 * ((float(random(100))/100) - 0.5) * InitialWeightMax;
    }
  }
  
  for( p = 0 ; p < PatternCount ; p++ ) {    
    randomizedIndex[p] = p ;
  }
  
  Serial.println("First test: ");
  testing();
  
  trainNN(); //обучение
  
  Serial.println("");  
  Serial.println("Final test: ");
  testing();
}
 
void loop() {
}

void neuron(){ //логика нейрона
  for(int i=0; i<outQ; i++) out[i] = 0.0;
    
  float s[hiddQ];
  for(int i=0; i<hiddQ; i++){
    s[i] = hNeuron[i][0][inQ]; //bias
    for(int j=0; j<inQ; j++){
      s[i] += hNeuron[i][0][j] * hNeuron[i][1][j];
    }
  }
  for(int i=0; i<outQ; i++){
    out[i] = oNeuron[i][0][hiddQ]; //bias
    for(int j=0; j<hiddQ; j++){
      oNeuron[i][1][j] = activateFunc(s[j]);
      out[i] += oNeuron[i][0][j] * oNeuron[i][1][j];
    }
    out[i] = activateFunc(out[i]);
  }
}

float activateFunc(float arg){
  float result;
  result = 1.0/(1.0 + exp(-arg));
  return result;
}

void trainNN(){
 /* if(task == "and"){
    for(int i=0; i<PatternCount; i++){
      learnSet[i].c = learnSet[i].a & learnSet[i].b;
      learnSet[i].d = !learnSet[i].c;
    }
  }
  else if(task == "or"){
    for(int i=0; i<PatternCount; i++){
      learnSet[i].c = learnSet[i].a | learnSet[i].b;
      learnSet[i].d = !learnSet[i].c;
    }
  }
  else if(task == "xor"){
    for(int i=0; i<PatternCount; i++){
      learnSet[i].c = learnSet[i].a ^ learnSet[i].b;
      learnSet[i].d = !learnSet[i].c;
    }
  }*/
  
  Serial.println("Start train!");
  int it = 0; //итерации
  float error;
  float hiddDelta[hiddQ];
  float outDelta[outQ];
  for(epoch=1; epoch < 2147483647; epoch++){
    it++;//увеличиваем на 1 итерацию
    for( p = 0 ; p < PatternCount ; p++) {
      q = random(PatternCount);
      r = randomizedIndex[p] ; 
      randomizedIndex[p] = randomizedIndex[q] ; 
      randomizedIndex[q] = r ;
    }
    error = 0.0;
    
    for(int q = 0; q < PatternCount; q++){
      p = randomizedIndex[q];

      //вводим обучающие значения на вход
      for(int i=0; i<hiddQ; i++){
        for(int j=0; j<inQ; j++){
          hNeuron[i][1][j] = Input[p][j];
        }
      }
      
      neuron(); //обрабатываем на нейроне

      //вычисляем ошибку на выходе
      for(int i=0; i<outQ; i++){ 
        outDelta[i] = (Target[p][i] - out[i]) * out[i] * (1.0 - out[i]);
        error += 0.5 * (Target[p][i] - out[i]) * (Target[p][i] - out[i]); 
      }

      //обратное распространение ошибки на скрытый слой
      for(int i=0; i<hiddQ; i++){ 
        float accum = 0.0;
        for(int j=0; j<outQ; j++){
          accum += oNeuron[j][0][i] * outDelta[j];
        }
        hiddDelta[i] = accum * oNeuron[j][1][i] * (1.0 - oNeuron[j][1][i]);
      }

      //коррекция весов скрытого слоя
      for(int i=0; i<hiddQ; i++){ 
        changeHiddNeuron[i][inQ] = LearningRate * hiddDelta[i] + Momentum * changeHiddNeuron[i][inQ];
        hNeuron[i][0][inQ] += changeHiddNeuron[i][inQ];
        for(int j=0; j<inQ; j++){
          changeHiddNeuron[i][j] = LearningRate * hNeuron[i][1][j] * hiddDelta[i] + Momentum * changeHiddNeuron[i][j];
          hNeuron[i][0][j] += changeHiddNeuron[i][j];
        }
      }

      //коррекция весов выходного слоя
      for(int i=0; i<outQ; i++){ 
        changeOutNeuron[i][hiddQ] = LearningRate * outDelta[i] + Momentum * changeOutNeuron[i][hiddQ];
        oNeuron[i][0][hiddQ] += changeOutNeuron[i][hiddQ];
        for(int j=0; j<hiddQ; j++){
          changeOutNeuron[i][j] = LearningRate * oNeuron[i][1][j] * outDelta[i] + Momentum * changeOutNeuron[i][j];
          oNeuron[i][0][j] += changeOutNeuron[i][j];
        }
      }
    }

    if(it == 100){
      Serial.print("Epoch: ");
      Serial.println(epoch);
      Serial.print("Error: ");
      Serial.println(error, 5);
      Serial.println();
      it = 0;
    }
        
    if( error < Success ) break; //пока error не равно Success, выполняем обучение
  } 
  Serial.println("End train!");
  Serial.print("Epoch: ");
  Serial.println(epoch);
  Serial.print("Error: ");
  Serial.println(error, 5);
}

void testing(){
  for( p = 0 ; p < PatternCount ; p++ ) { 
    Serial.println(); 
    Serial.print ("  Pattern: ");
    Serial.println (p);      
    Serial.print ("  Input ");
    for( i = 0 ; i < inQ ; i++ ) {
      Serial.print (Input[p][i], DEC);
      Serial.print (" ");
    }
    Serial.print ("  Target ");
    for( i = 0 ; i < outQ ; i++ ) {
      Serial.print (Target[p][i], DEC);
      Serial.print (" ");
    }
    for(int i=0; i<hiddQ; i++){
      for(int j=0; j<inQ; j++){
        hNeuron[i][1][j] = Input[p][j];
      }
    }
    neuron();
    Serial.print ("  Output ");
    for( i = 0 ; i < outQ ; i++ ) {       
      Serial.print (out[i], 5);
      Serial.print (" ");
    }
  }
  Serial.println();  
}

