# Algoritmo MAPA
A heuristic algorithm based on multi-assignment procedures for nurse scheduling.

# Autores
Ademir Aparecido Constantino, Dario Landa-Silva, Everton Luiz de Melo, Candido Ferreira Xavier de Mendonça, Douglas Baroni Rizzato & Wesley Romão 

# Proposta 
Algoritmo com duas fases, uma construtiva e uma de melhoramente que utiliza solução de problemas de atribuição para encontrar uma solução ótima para o problema de escalonamento de enfermeiros

#Executando 
+ Para executar no Clion:
  Clean -> Makefile 
  Run -> # ./main1 (Criar um script de execução)

+ Para executar no Local:
  Clean -> # gcc -o main1 libraries/dinamicList.c libraries/strings.c libraries/nspLib.c libraries/hungarian/hungarian.c libraries/apc/apc.c main1.c
  Run -> # ./main1 

+ Para executar no NetBeans:
  Compilador permite utilizar o makefile e o execute da propria IDE