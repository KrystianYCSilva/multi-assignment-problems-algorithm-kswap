# **Arquiteturas Heurísticas Avançadas em Otimização Combinatória: O Algoritmo Baseado em Problemas de Multi-Atribuição (MAPA) e suas Variantes em Escalonamento e Roteirização**

O campo da otimização combinatória tem testemunhado uma evolução paradigmática com o desenvolvimento de técnicas que buscam equilibrar a eficiência computacional e a qualidade das soluções em problemas de larga escala. Entre essas inovações, o Algoritmo Baseado em Problemas de Multi-Atribuição, amplamente conhecido pela sigla MAPA (*Multi-Assignment Problem-based Algorithm*), destaca-se como uma abordagem heurística determinística de alto desempenho. Concebido primordialmente por pesquisadores vinculados à Universidade Estadual Paulista (UNESP), como Ademir Aparecido Constantino e Wesley Romão, em colaboração com Dario Landa-Silva da University of Nottingham, o MAPA foi projetado para enfrentar a complexidade inerente aos problemas de escalonamento de pessoal e motoristas, que são classificados como NP-difíceis.1 A essência desta abordagem reside na decomposição de problemas globais complexos em uma série de subproblemas de atribuição linear, os quais podem ser resolvidos em tempo polinomial, garantindo rapidez e robustez mesmo em instâncias com milhares de tarefas.3

## **Fundamentos Teóricos e a Estrutura do Problema de Atribuição**

Para compreender a eficácia do MAPA, é imperativo analisar o Problema de Atribuição Linear (LAP), que serve como motor fundamental do algoritmo. No LAP clássico, o objetivo é encontrar uma correspondência um-para-um entre dois conjuntos de mesma cardinalidade (por exemplo, funcionários e turnos) de modo a minimizar o custo total.5 Matematicamente, o LAP é expresso como:

Minimize:

![][image1]  
Sujeito a:

![][image2]  
![][image3]  
![][image4]  
Embora o LAP seja tratável computacionalmente através de métodos como o algoritmo húngaro ou algoritmos de leilão, os problemas de escalonamento do mundo real, como o *Nurse Scheduling Problem* (NSP) e o *Bus Driver Scheduling Problem* (BDSP), envolvem restrições temporais e contratuais que impedem uma resolução direta por atribuição única.4 O MAPA supera essa limitação ao modelar o horizonte de planejamento como um grafo multipartite ponderado, onde cada partição representa um período ou uma tarefa, e as arestas representam a viabilidade de transição entre estados.4

### **A Representação em Grafo Multipartite e a Modelagem de Custos**

A modelagem proposta por Constantino, Mendonça Neto e outros pesquisadores utiliza grafos multipartite para capturar a estrutura sequencial das jornadas de trabalho.4 Em um cenário de escalonamento de motoristas, cada "camada" do grafo pode representar um conjunto de tarefas que devem ser executadas cronologicamente. O peso de cada aresta não é meramente um custo financeiro, mas uma função complexa que incorpora penalidades por violações de restrições flexíveis (*soft constraints*) e o custo operacional de manter um motorista ou enfermeiro em atividade.4

A tabela a seguir apresenta uma comparação das dimensões de instâncias abordadas na literatura em contraste com as capacidades demonstradas pela abordagem baseada em multi-atribuição, evidenciando o salto de escala proporcionado por esta metodologia.

| Referência | Problema | Abordagem | Número de Tarefas | Número de Jornadas |
| :---- | :---- | :---- | :---- | :---- |
| Santos e Mateus (2007) | BDSP | Genético \+ CG | 138 | \- |
| Yunes et al. (2005) | BDSP | SCP \+ CG | 246 | ![][image5] 50 |
| Li et al. (2015) | BDSP | Hiper-heurística | 500 | 145 |
| Mauri e Lorena (2007) | BDSP | Heurística \+ CG | 500 | 153 |
| **Abordagem MAPA/GraphBDSP** | **BDSP** | **Heurística de Multi-atribuição** | **2.313** | **340** |

Esta capacidade de lidar com instâncias quatro vezes maiores do que o reportado anteriormente na literatura de referência justifica o foco na simplicidade e na eficiência assintótica do algoritmo.4

## **A Arquitetura do Algoritmo MAPA Padrão**

O MAPA é definido por sua natureza determinística, o que significa que, ao contrário de meta-heurísticas estocásticas, ele produzirá a mesma solução para a mesma instância em cada execução.2 Esta propriedade é extremamente valorizada em ambientes corporativos onde a reprodutibilidade das escalas de trabalho é necessária para a transparência e aceitação sindical. O algoritmo opera fundamentalmente em duas fases distintas: uma fase construtiva e uma fase de melhoria.2

### **Fase 1: Construção da Solução Inicial**

Na fase construtiva, o MAPA constrói um cronograma completo resolvendo sucessivos problemas de atribuição, um para cada dia ou etapa do período de planejamento.1 A cada iteração, o algoritmo avalia o estado atual de cada recurso (enfermeiro ou motorista) e calcula o custo de atribuir-lhe um novo turno ou tarefa, considerando o histórico de trabalho acumulado para garantir a viabilidade em relação a restrições de dias consecutivos e períodos de descanso.8

A lógica da fase construtiva pode ser detalhada conforme o seguinte processo: para cada dia do horizonte de planejamento, uma matriz de custo é gerada onde cada entrada representa a "preferência" ou o "custo de penalidade" de um par funcionário-turno. O subproblema de atribuição resultante é resolvido, e as atribuições são fixadas antes de avançar para o próximo dia.2 Se o número de enfermeiros exceder a demanda de turnos, turnos "fictícios" ou de folga são adicionados para manter a matriz quadrada e permitir o uso de algoritmos de atribuição eficientes.8

### **Fase 2: Procedimentos de Melhoria e Recombinação**

A fase de melhoria não utiliza buscas locais tradicionais baseadas em trocas simples, mas sim uma técnica robusta conhecida como *Cut-and-Recombine* (CRP). Este procedimento re-resolve problemas de atribuição em larga escala para otimizar as conexões entre diferentes segmentos do cronograma.2

A inovação do CRP reside na sua capacidade de avaliar simultaneamente ![][image6] combinações de jornadas. O algoritmo seleciona um ponto de corte no cronograma, divide todas as jornadas em partes esquerdas e direitas, e então encontra a combinação ótima de partes direitas com partes esquerdas para minimizar o custo total ou equilibrar a carga de trabalho.4 Esse processo é repetido para múltiplos pontos de corte, permitindo que a solução "escape" de ótimos locais que seriam intransponíveis por métodos que movem apenas um funcionário por vez.8

## **Variante GraphBDSP: Escalonamento de Motoristas de Ônibus em Larga Escala**

Uma das aplicações mais proeminentes da lógica MAPA é o algoritmo GraphBDSP, desenvolvido para resolver problemas de escalonamento de motoristas em empresas de transporte público brasileiras.4 O BDSP envolve a cobertura de todas as viagens de uma frota com o menor custo possível, respeitando leis trabalhistas rigorosas e acordos sindicais.

### **Regras Operacionais e Restrições Laborais**

O GraphBDSP foi desenhado para integrar nativamente as complexas regras do cenário brasileiro. A análise dos trabalhos de Constantino e Romão revela um conjunto de parâmetros fundamentais que regem a formação de jornadas de trabalho factíveis 4:

| Parâmetro | Significado | Valor de Referência |
| :---- | :---- | :---- |
| **![][image7]** | Tempo máximo de condução contínua (stretch) | 360 min |
| ![][image8] | Tempo mínimo de repouso entre stretches | 90 min |
| ![][image9] | Tempo máximo de descanso (acima disso vira tempo pago) | 300 min |
| ![][image10] | Tempo mínimo pago por jornada | 432 min |
| ![][image11] | Horas extras máximas permitidas | 120 min |
| ![][image12] | Tempo total de amplitude da jornada (spreadover) | 780 min |

Estas regras são codificadas na função de custo do algoritmo de atribuição, garantindo que qualquer conexão entre tarefas que viole estas normas receba uma penalidade proibitiva.4

### **Pseudo-código do Algoritmo GraphBDSP**

Abaixo, apresenta-se o pseudo-código estruturado do GraphBDSP, integrando a lógica de multi-atribuição aplicada ao contexto de transporte 4:

Algoritmo GraphBDSP

Entrada: Conjunto de tarefas (tasks), Matriz de custos, Restrições trabalhistas (Rules 1-5)

Saída: Conjunto de jornadas (duties) otimizado

FASE 1: CONSTRUÇÃO (MAPA Inicial)

1. Organizar as tarefas cronologicamente em um grafo multipartite com L camadas.  
2. Para cada intervalo m entre camadas consecutivas (Tm e Tm+1):  
   a. Construir a Matriz de Custo C para o Problema de Atribuição Linear.  
   b. Resolver o Problema de Atribuição (LAP) para conectar tarefas de Tm com Tm+1.  
   c. Verificar se a conexão respeita o tempo de condução (st) e pausas (rt).  
   d. Armazenar as conexões válidas como jornadas parciais.  
3. Consolidar todas as conexões para gerar a solução inicial factível (S\_init).

FASE 2: MELHORIA (Refinamento Heurístico)

4\. Enquanto houver melhoria no custo total:

a. Procedimento A: Recombinação Multi-atribuição (CRP)

i. Selecionar um ponto de corte (d\_cut) no conjunto de jornadas.

ii. Dividir jornadas em partes anteriores e posteriores ao corte.

iii. Criar matriz de custo E para todas as possíveis recombinações.

iv. Resolver o LAP para encontrar a recombinação de custo mínimo.

b. Procedimento B: Ajuste de Jornadas Longas

i. Identificar jornadas que excedem o tempo normal de trabalho (overtime).

ii. Identificar jornadas com capacidade ociosa (idle time).

iii. Tentar transferir tarefas de fim de jornada para jornadas ociosas.

5\. Retornar o melhor conjunto de jornadas encontrado (S\_best).

Esta estrutura demonstra como o GraphBDSP utiliza o MAPA como um motor de otimização local robusto, transformando um problema global NP-difícil em uma sequência de decisões ótimas locais que, combinadas, resultam em uma solução global de alta qualidade.4

## **Variante NSPBPS: Equilíbrio e Justiça no Escalonamento de Enfermeiros**

Uma crítica comum aos algoritmos de otimização é que a minimização do custo total pode resultar em soluções "injustas", onde alguns funcionários recebem escalas excelentes enquanto outros são sobrecarregados com turnos indesejados para atingir o ótimo global. Para mitigar isso, Constantino, Landa-Silva e seus coautores propuseram a variante NSPBPS (*Nurse Scheduling Problem with Balanced Preference Satisfaction*).8

### **O Problema de Atribuição de Gargalo (BAP)**

Diferente do MAPA tradicional que foca na soma dos custos (LAP), o NSPBPS utiliza o Problema de Atribuição de Gargalo (*Bottleneck Assignment Problem* \- BAP). O objetivo do BAP é minimizar o valor máximo de qualquer atribuição individual, promovendo o equilíbrio.8

A formulação do BAP aplicada à escala de enfermeiros é:

![][image13]  
Ao adotar esta métrica, o algoritmo garante que o enfermeiro "menos favorecido" tenha o menor nível de insatisfação possível, resultando em uma distribuição mais homogênea das violações de preferência.8

### **Integração com Variable Neighbourhood Search (VNS)**

A variante de equilíbrio evoluiu para uma hibridização com a meta-heurística VNS. Neste contexto, o MAPA fornece as estruturas de vizinhança que o VNS explora sistematicamente. O pseudo-código para esta variante foca nos procedimentos CRP e k-Swap 8:

Procedimento CRP (Cut-and-Recombine para Equilíbrio)

Início

Para cada dia de corte (dcut) de 1 até dmax \- 1:

1\. Construir matriz de custo \[cdcut\_ij\];

// Onde c\_ij é o custo de atribuir a cauda da jornada j ao enfermeiro i.

2\. Resolver o BAP para a matriz de custo;

3\. Recombinar os roteiros conforme a solução do BAP;

Fim-Para

Fim

Procedimento k-Swap

Início

Para cada dia de corte (dcut):

1\. Dividir o cronograma em: Lado Esquerdo, Bloco-k (tamanho k) e Lado Direito.

2\. Tentar permutar os Blocos-k entre enfermeiros para minimizar a insatisfação máxima.

3\. Se uma melhoria for encontrada, atualizar a escala global.

Fim-Para

Fim

Estes procedimentos permitem que o algoritmo navegue por vizinhanças complexas de forma eficiente, mantendo a factibilidade das escalas de trabalho enquanto busca a equidade entre os profissionais de saúde.8

## **Desempenho Computacional e Validação Experimental**

A eficácia do MAPA foi rigorosamente testada utilizando a biblioteca NSPLib (*Nurse Scheduling Problem Library*), que contém cerca de 250.000 instâncias de teste com variados níveis de complexidade.2 Os experimentos realizados por Rizzato, Romão e Mendonça indicam que o MAPA supera consistentemente os melhores resultados conhecidos na literatura para instâncias de grande porte.2

### **Resultados em Larga Escala e Simplicidade Algorítmica**

Um dos grandes diferenciais do MAPA é o cumprimento dos critérios de Cordeau et al. (2002) para uma boa heurística: simplicidade, flexibilidade, precisão e velocidade.3 O algoritmo não requer o ajuste exaustivo de parâmetros (como taxas de mutação em algoritmos genéticos), facilitando sua adoção prática.

A tabela abaixo resume o desempenho do MAPA em diferentes contextos, destacando sua robustez 2:

| Contexto de Aplicação | Dimensão da Instância | Desempenho Relativo | Característica Principal |
| :---- | :---- | :---- | :---- |
| Enfermeiros (NSPLib) | 248.640 instâncias | Superior a meta-heurísticas em 90%+ dos casos | Determinismo e rapidez 2 |
| Motoristas (Empresa BR) | 2.313 tarefas / 340 motoristas | Solução factível em segundos | Gerenciamento de regras laborais complexas 4 |
| Roteirização (Artur Nogueira) | 20-25 carteiros (VRP) | Otimização multi-objetivo | Integração com GIS e dados reais 10 |

### **Implementação e Requisitos de Sistema**

As implementações originais do MAPA, reportadas em periódicos como *Annals of Operations Research*, foram desenvolvidas utilizando a linguagem de programação Pascal.3 Os testes foram executados em ambientes de processamento padrão (como Xeon quad-core de 3.2 GHz), demonstrando que o algoritmo não exige infraestrutura de supercomputação para resolver problemas urbanos complexos, sendo ideal para sistemas de gestão empresarial (ERP) e despacho de frotas.3

## **Extensões para Roteirização de Veículos (VRP) e Ferramentas Associadas**

Além do escalonamento de pessoal, a lógica de atribuição do MAPA foi estendida para o Problema de Roteirização de Veículos (VRP). Douglas Baroni Rizzato e Wesley Romão desenvolveram o VRPBench, uma ferramenta para criação e visualização de instâncias de VRP baseadas em grafos planares e espaços euclidianos.10

### **O Estudo de Caso de Artur Nogueira**

Utilizando o VRPBench, o MAPA foi aplicado ao serviço de entrega de correspondências na cidade de Artur Nogueira, São Paulo. O problema foi modelado como um VRP multi-objetivo onde cada carteiro é tratado como um veículo e cada ponto de entrega como um cliente em um grafo ponderado.10 A abordagem de multi-atribuição permitiu otimizar não apenas a distância percorrida, mas também o equilíbrio da carga de trabalho entre os carteiros, refletindo a mesma filosofia de "well-being" aplicada ao escalonamento de enfermeiros.10

As principais variantes de VRP abordadas pela escola de pesquisa da UNESP incluem:

* **CVRP (Capacitated VRP):** Onde a capacidade de carga do veículo (ou peso que o carteiro pode carregar) é limitada.10  
* **VRPTW (VRP with Time Windows):** Onde as entregas devem ocorrer em janelas de tempo específicas, similar aos turnos de enfermagem.10  
* **MDVRP (Multi-Depot VRP):** Expansão do problema para múltiplos pontos de partida, comum em grandes operações logísticas.10

## **Contribuições dos Autores e Ecossistema de Pesquisa**

O sucesso do MAPA é fruto de uma colaboração acadêmica sinérgica. Ademir Aparecido Constantino e Candido Ferreira Xavier de Mendonça lideraram a base teórica na UNESP, focando na modelagem de grafos e na eficiência dos subproblemas de atribuição.1 Wesley Romão e Douglas Baroni Rizzato foram fundamentais na implementação prática e no desenvolvimento de *benchmarks* reais, como o caso da empresa de ônibus brasileira e o projeto de roteirização urbana.4

Dario Landa-Silva, da University of Nottingham, contribuiu com a expertise em meta-heurísticas de busca local e VNS, permitindo que o MAPA evoluísse de uma heurística construtiva para um componente integral de algoritmos híbridos de ponta.1 Everton Luiz de Melo também desempenhou papel crucial na documentação e análise de desempenho do algoritmo em contextos de medicina e gestão hospitalar.1

## **Implicações Broder e Perspectivas Futuras**

A trajetória do MAPA demonstra que a simplificação inteligente de problemas NP-difíceis através da decomposição em subproblemas polinomiais é uma via fértil para a otimização prática. Diferente de abordagens de "caixa-preta", o MAPA oferece uma lógica interpretável, onde gestores podem entender como os custos e restrições influenciam a escala final.3

A evolução futura do MAPA aponta para a integração com técnicas de aprendizado de máquina, visando "aprender" as matrizes de custo ideais a partir de dados históricos de escalas manuais, e para o desenvolvimento de versões paralelas que possam tirar proveito de arquiteturas de múltiplos núcleos (*multicore*) e GPUs para resolver instâncias globais em milissegundos.5

## **Conclusões**

O Algoritmo Baseado em Problemas de Multi-Atribuição (MAPA) consolidou-se como uma ferramenta indispensável na caixa de ferramentas da pesquisa operacional contemporânea. Sua capacidade de gerar soluções de alta qualidade, de forma determinística e sem a necessidade de parametrização complexa, o posiciona como uma solução robusta para o escalonamento de pessoal e roteirização de frotas em larga escala.2 As variantes GraphBDSP e NSPBPS exemplificam a versatilidade da arquitetura, adaptando-se tanto a objetivos de minimização de custo puro quanto a critérios modernos de justiça e bem-estar do trabalhador.4 Através do trabalho contínuo de pesquisadores como Constantino, Romão e Landa-Silva, o MAPA continua a definir o estado da arte na resolução de problemas que impactam diretamente a eficiência do transporte público e a qualidade do atendimento nos sistemas de saúde.

#### **Trabalhos citados**

1. A heuristic algorithm based on multi-assignment procedures for nurse scheduling, acesso a fevereiro 13, 2026, [https://www.semanticscholar.org/paper/A-heuristic-algorithm-based-on-multi-assignment-for-Constantino-Silva/3a172043f976d2199a2b35affa3f7d6ec2d3b19d](https://www.semanticscholar.org/paper/A-heuristic-algorithm-based-on-multi-assignment-for-Constantino-Silva/3a172043f976d2199a2b35affa3f7d6ec2d3b19d)  
2. A heuristic algorithm based on multi-assignment procedures for nurse scheduling | Request PDF \- ResearchGate, acesso a fevereiro 13, 2026, [https://www.researchgate.net/publication/257516540\_A\_heuristic\_algorithm\_based\_on\_multi-assignment\_procedures\_for\_nurse\_scheduling](https://www.researchgate.net/publication/257516540_A_heuristic_algorithm_based_on_multi-assignment_procedures_for_nurse_scheduling)  
3. A heuristic algorithm based on multi-assignment procedures for nurse scheduling \- University of Nottingham, acesso a fevereiro 13, 2026, [https://people.cs.nott.ac.uk/pszjds/research/files/dls\_aor2013.pdf](https://people.cs.nott.ac.uk/pszjds/research/files/dls_aor2013.pdf)  
4. (PDF) Solving a Large Real-world Bus Driver Scheduling Problem with a Multi-assignment based Heuristic \- ResearchGate, acesso a fevereiro 13, 2026, [https://www.researchgate.net/publication/317017247\_Solving\_a\_Large\_Real-world\_Bus\_Driver\_Scheduling\_Problem\_with\_a\_Multi-assignment\_based\_Heuristic](https://www.researchgate.net/publication/317017247_Solving_a_Large_Real-world_Bus_Driver_Scheduling_Problem_with_a_Multi-assignment_based_Heuristic)  
5. Multidimensional assignment problem \- Wikipedia, acesso a fevereiro 13, 2026, [https://en.wikipedia.org/wiki/Multidimensional\_assignment\_problem](https://en.wikipedia.org/wiki/Multidimensional_assignment_problem)  
6. Parallel Auction Algorithm, acesso a fevereiro 13, 2026, [https://www.cs.columbia.edu/\~sedwards/classes/2024/4995-fall/reports/assignment-report.pdf](https://www.cs.columbia.edu/~sedwards/classes/2024/4995-fall/reports/assignment-report.pdf)  
7. A Survey of the Nurse Rostering Solution Methodologies: The State-of-the-Art and Emerging Trends \- IEEE Xplore, acesso a fevereiro 13, 2026, [https://ieeexplore.ieee.org/iel7/6287639/6514899/09780256.pdf](https://ieeexplore.ieee.org/iel7/6287639/6514899/09780256.pdf)  
8. A Variable Neighbourhood Search for Nurse Scheduling with Balanced Preference Satisfaction \- ResearchGate, acesso a fevereiro 13, 2026, [https://www.researchgate.net/publication/282795177\_A\_Variable\_Neighbourhood\_Search\_for\_Nurse\_Scheduling\_with\_Balanced\_Preference\_Satisfaction](https://www.researchgate.net/publication/282795177_A_Variable_Neighbourhood_Search_for_Nurse_Scheduling_with_Balanced_Preference_Satisfaction)  
9. NSPLib: a Nurse Scheduling Problem Library: a tool to evaluate (meta-)heuristic procedures, acesso a fevereiro 13, 2026, [https://www.semanticscholar.org/paper/NSPLib%3A-a-Nurse-Scheduling-Problem-Library%3A-a-tool-Vanhoucke-Maenhout/db7c0f19c9a6e14775e85ce70fe4565779d4f768](https://www.semanticscholar.org/paper/NSPLib%3A-a-Nurse-Scheduling-Problem-Library%3A-a-tool-Vanhoucke-Maenhout/db7c0f19c9a6e14775e85ce70fe4565779d4f768)  
10. VRPBench: A Vehicle Routing Benchmark Tool \- arXiv, acesso a fevereiro 13, 2026, [https://arxiv.org/pdf/1610.05402](https://arxiv.org/pdf/1610.05402)  
11. (PDF) VRPBench: A Vehicle Routing Benchmark Tool \- ResearchGate, acesso a fevereiro 13, 2026, [https://www.researchgate.net/publication/309283944\_VRPBench\_A\_Vehicle\_Routing\_Benchmark\_Tool](https://www.researchgate.net/publication/309283944_VRPBench_A_Vehicle_Routing_Benchmark_Tool)  
12. Dynamic multicriteria optimization for the nurse scheduling problem \- ResearchGate, acesso a fevereiro 13, 2026, [https://www.researchgate.net/publication/389686446\_Dynamic\_multicriteria\_optimization\_for\_the\_nurse\_scheduling\_problem](https://www.researchgate.net/publication/389686446_Dynamic_multicriteria_optimization_for_the_nurse_scheduling_problem)  
13. MODELLING DISTRIBUTION ROUTES IN CITY LOGISTICS BY APPLYING OPERATIONS RESEARCH METHODS, acesso a fevereiro 13, 2026, [https://hrcak.srce.hr/file/412882](https://hrcak.srce.hr/file/412882)

[image1]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAA3CAYAAACxQxY4AAAEFklEQVR4Xu3dTcilYxgH8FsoohgUb0TyXfLRGynDBmFBFhbKUr4iC5qZmlKD7Kwp0tQUSkmaUj7SYIqthZQskCwpZWEWuK6e5+nc7935es77ls7r96t/5zn3fc61vnq+rlIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABgtAsjByPnRDb6rOqRMql1rGyvFgAAvWyujvbHm5Hzqr2xLi6TWsfL9moBAFD5MHJm//lMszfG3jKpdVvZXi0AACqHStdkvRO5e+vWKM+WSa3ny/ZqAQAAAAAAAAAAsLv8EfknciLy84zkfp3fI9fmnyvXl0mt9v9Dfuv321oAAMxxVpk0T3m8yEmRHyLftRtlUuvT/niRodb57QYAAFv9WrpGa3+7MUO+p+2zdrG3Sq2n20UAALY6JfJa6RqtS5q9eU5tF0pXazhjt2ytPBs3rRYAAI28nywbrefajZFuLpNaAADsoCfKzj0IMNTK5g0AgB30fekardfbjRVsp9aVkTciD0aONHvTvB95LPJ5ZF/ko63bAAC7Rz65mQ8MZKN1ebM3VtYa7mcbUyvHWH3dH58eubU/vq50NQe395917b8iZ5SueRvcUR1nvY3qOwDAWsqGKZusp9qNFXxcxte6M/JyuxierI7z4Ya7qu+DH5vv+bu61kVFwwYA7AL5xOYqlzGnyTpja+XA+M3qe54xe6F0l0gHeTw8WZpNWDZv10QO9GuP9p+vRl7sj++LvNcfAwCsrXwn2pft4oqyVjZTY+Vlz18ib0a+KN1lzFsin/T7F0Ruiuzpv++NvBU5HPk28kHpmrc8u3Z/5N3+d9ngHeyPAQDW1jdl+fenLZK1VnVa5NwyuWftUJ/B42Xr/WzDb/Nybr3+duSh/jhrHq32AADWzpgGK5uiWSOlsuEbUysbqlm1Bjnu6qXq+/HqeJ6cyDBcEs174/KeOgCAtZOTBvJyY31map68HJnD3KfJWvmU6ZhaD7eLc+TrO/Ky5tiX++bs03zlx43tBgDAOhjzYEBeejxRunvMpsk6y46ZGmpd2m7M8UDklbJ8QzjIJvKydhEAYB0M71wbk78j9+SfK/X728YkawEAAAAAAAAAAEA9c3OakyM3tIsz5FSBebLWve0iAACzzZrNOchXaeRLZxc1Yqmd3zlN1vqpXQQAYLoc9ZQzNxe9jiOnCCzTsOWcz0UjoLLWsXYRAIDprijdzM08MzbPMg3bMOczJwzMo2EDABghG7WcuZnyZbYbTc7u95Zp2FLO+TxcuneztbUyScMGADDCVaU7I3Z1u1HJBwSORL4q3RD1eXLO52a72Mhaf0b2tRsAAEy3qAkbI+eS7mkXAQD47+Ul0FXmfAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD8z/wLGWWyD1Pi9UgAAAAASUVORK5CYII=>

[image2]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAA2CAYAAAB6H8WdAAAFEklEQVR4Xu3dS6jtUxwH8CUUIW+iK4/II8+UoighDJRQyISEEgYUxeRO5FEGuB4ZEIUyoTwm5DFSKBmYKANSyoyikMf69t+bddbd+5x97tl7n0OfT33b+7/Wvvuedc5g/1pr/dcuBQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgC3k0JrHa06veblm28puAAA22wk1l9XsW/NCzW0ruwEA2Gx71Lw2ep7HM5s+AAC2gMNrPhg9f6nmvqYPAAAAAAAAAAAA5iF71f4a5buabyfkh+Y14/yZfwwAwHI8V4Yi7Kuao7u+aT6pObVvBABgMQ4uQwGWoi3F2yz2rtlRs1vfwYbtWXNA37jF5Hy+BABYsvFy5zl9xxT5wN6rb9xEx/QNG3R931DtX/N813ZTd70R19Vc2DeW+Y9tHsaFfs7sAwCWJMuhWRZN0fZf+hA+uea8MuzBm6fMHp7dtT1ac27Xdkl3PckRTVaT8+7a1yxqbPPyYjHTBgBLl+IjBdvVfccWlyLnm75xDh7orp8t6ytQUvR9Woal5qdrrlnZvZO+YItFjW0eFGwAsEneK0PRlsf1eqUMRc4XNQ/WvF6Ws2y6qKLmnvLvzNixNcc1fRnr3TVvNm2tq2qe6hvXsJUKtm01j9XcWIYxPlGGGb+Wgg0ANtEfZSjazu87VnFQGYqzM2p+KsPm+VOa/oua51fUXNBcb9SiiposDY+LtHa27ZBR3z41bzftrczGHVVWLofmGJVJ8rt6uOb7vqMsbmyrydhSPN5fc8Ko7eKay/95xSB7+j4qw98cAFiyZ8pQsD3ed8zghpqfu7YUAO1er8y+zbL3a1aLLGpuHz32NxtEirkUNZO8X4al0D6TZOYqP//NfUdZ7NjWkmJ0PEO6vaycYYws+X4+CgCwZPfWvNs3riGzLbfWfFnzwqgt15H9W5lFiszYpMgZX/eyz2tacpzIJJOKmizVpaDYqIfKsPG/vwEhUnj2RczYI2X9RWneb5Yl0Ulj271MXn4+srvO647p2mLSUSKZaR37peawMvw+xrIkul9zDQAsUfagzXqI7liWy7Ln6fcyLKelgDu+DIVFlszG73dXzWc1p42u52Fc1LRFTMZwbnO9q/Ieb5Sdi8UUR+0MVC/jnbZcOs1qe9jWGltm514tK+/wzdEbvzbXkdf1dwKfWIY7UY9t2qJdok3/bTXXNm0p2OxhA4BNkKM9dlVmYCJFTGZyxrJBv5UZuH6GaBHu6Bt2UV/IRArUdgZqmuz9+roMX/OVx9VMKtimmdfYxm4pO+9Ra4vRPO//Zgo2AFiyzIhlGbT/UJ4mH+6ZmZlF9nOdNHqeDfuTlhfnLbN6/azYPORnz92vmX2a9/EnKdimLbG2FjG2LOHO+rePvPblomADgKVKsba9b1xFjnqYdtdjr52pScFzYHO9KLkTdRFS2GaTfXsH7LycVYYiaC3zHluWRzOu9ciM3G99IwCwGJkpyU0Gs35g5yiLH8vOe6jWkv/nyprL+g4AAFY3PnNtvQEAYAkyW9afEzZLduV8NgAAAAAAAAAAAJgmZ3vt6BsbOXPtrZoPu3YAAJYgR268U3Nm39HJ+V8f9o0AACzeyWX4QvbtNZeWoShrk5m1ULABAGyifNNBDs/NtxLk+yzbjL/RQMEGALCJnqy5s29sHFDzcc3PZe2lUwAAFmC/sr4v/wYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADgf+9vbpC+p2lD6aYAAAAASUVORK5CYII=>

[image3]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAA3CAYAAACxQxY4AAAFF0lEQVR4Xu3dW8jkYxwH8EcOETlnnSKEC9mISLiRRDlcEIokcj7kBtlSW5JDOcSKkM0WRS4USsvFlhuHCyml1hU5lBs3VlEOz69npv3Pb//z7jvmnXcGn09925nn+b/v/Gf2rfn1HP7/UgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFgw19Zsrdml5v7UBwDAAji+ZnNpBdvG1AcAwALYrebSQd6suXK0GwCAeVtTc0xpBdummv1GuwEAAAAAAAAAAGBqL9f8NchPNd/25LvOMcNsKG0HKQAAq+Dy0oqwP3LHGPuWdvyduQMAgNmIkbLhyNnRqW+cq2o+z41MLXbnLrL4W1n0cwSA/6xTa34urWhb7lTn7rlhzq7PDVO6OjeUdnmTgweP9645odM3jXit33Jjaf8vF+TGBfBr6f98AIAZiwIsCravao5MfYvssprTa77JHVN6OD2PQvbxzvMo3HZWtB5Sc9ggu6a+rrj9Vz7/S2q2DfoWTZzrIp4XAPwvfFla0fZi7vgXyAXPtC5Mz2Mq8IXUNs5eNffWvFpakRejUdE2Tl/BFhRsAMAOuuvZzkh9OxM/+3zNdaUVKffVfD1yxGz1FTzTurZsHyF7vebY0kbVHqz5rHNcV9zWKzZyTGKRCrYYXX2ltJHWd2quqfl+5AgFGwDMXWw82FomW88W7h78G2u73qo5u7TpyrC2jK55OrPzeKX0FTzTera0Ii0MR9xuqzmrtHVcfWIqc1jkdTPuszyq5sea83JHmU/BFq8XU7lxmZehvIM4pqDjnKO4W2qqFwCYobiHaBRsh+eOZYgCJxcZUeTcnJ6vtFkUbFFk3l5GNxuE9aVdn65PFGwxpZyzf/egjtdqbiz96+HmUbCFPWve6zyP4qwrpnfvKa0wX6mNFwDAhGI6dNIp0Rtq9ql5puaI0kZpniptxO2lsv0G8jFd+vTgcXZyzRVjclFZeh1YLtji9fuKoEk9UnbcgPB7zfmlFW5ZvG5kEvH78/mHvoItF37xHvsuxxJr7rI4Ln8m+feFeG/xeYcYTYzCNT6HPQZtca7rBo8BgDn4qPyzi+LGAvv3a+4qbeTlgdKKg+NrPqg5oObA0qbRPh78zEqI8/2ltBHBGPW6ddD+Z82Hw4OmEEVivoRFvM7bZfw058U1n5RWwC5H3xq2H8r29YTxHkN8dvG+umKkKwrIrjjui7K9SB6K42L0tCuOi1HBoZjifKO0UbZwTs27ZbTQs4YNAOYovuBjs8C4QmQp8UU//JLPI0zrO49j9GZz5/msxBRmrEFbCTFK2BWFWN/IVFfcESIKxuEtvg4a7R7RV7CNc1NuWAHD0bShXGjmc1ewAcCcRJEWI2HLdVxuGCO+7B+qOXfwPHYe5lGeWYidjiflxgUVxU9eJzbOY7lhSrGZYtICPUYYFWwAsMriMhQxNbZcT9Z8mhuXYVPNc6Vd+oJRsZM0pooXeddlrGGLc8wjbgDAKojLeCx3k0FcfiLWVa1P7cvxRNlx0TsAAEvoXnNtksTmgqV2bAIAAAAAAAAAAMCk7sgNPeLm7gAAzMmG3NBxYmnXUNuS2gEAWAVxqY24F+ja3NFjS24AAGD2YvQsLth6Te7osSU3AACwOuLem6eVdj/QuOp+N2s6x23pPAYAYBXdUpa+p2Tc7PzRmm01p6Q+AABmLAq1jbkRAIDFcGjNutKmRAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABg4G+CbMHjQpJBvwAAAABJRU5ErkJggg==>

[image4]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAACiElEQVR4Xu3dPcjNURwH8CMvEXktJQZ5C4PFYJAyWCQskoVBIcpLBqKUsqC8lhQhFguDlIThGZXFZLFZlGKz4nf87+05/+Pe59HjWR7386lv9/zP+d/uHX/9zrn/mxIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABMGIvqiTGaFFkQmVovAAAwNucjX6q52ZE7kXPVfD+7I0uruU2RPdUcAACFWanpmuXkcT8PI0PF9ZTIk+L6ZTGuzYhsjnyPrG8v/f7c09UcAAAdayNvUtMlOxJZ015uqQu2ZaldaOVibDQKNgBgIOSzX7cj+yKXI6ciH1t3/J2nkTn15Ajqgm17ahdaP4txPwo2AGAgHOu8zkzNluTGyM7O3LrUPg+2oRiX8tbn6jS8FdrN9PKmwsrIh9TuwJ1M41OwZZ8jFyOT6wUAgIms3pLMDkcOVte95ILtQWq2QstsLW8qPItsS013r2u8OmzZidT8oGFVvQAAMBHtT03BdTOyOLIwci01Hbe7aXibM2+XXu+MezlUT4wid/OGiuv82fk7dH0txnOLcalXwZY7e2erOQCACS2fJcu/yDyamiLqTGqeZZa3LV9H5kXmR5ZE3nbe00u+53Fkeb3QR32GLXuRmq7c/dR8XpZff6SmA9eVi8lvqenC5Vwt1pxhAwD+O/mcV/esWe6ulc4X4y2RV8V1L3mL80bkUyd728stvQq2/F12pT+7ZgdS/+3VmoINABgY+V8DLqTmQbTZ88iO4eV/lgu2d/VkH5dS+7zbSHJHTsEGAAycR5FbqXm47XhaEXlfT47RtMi9yPF6AQBgEFxJ/qMTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4B/9Ar7pUgY6fE/KAAAAAElFTkSuQmCC>

[image5]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA0AAAAUCAYAAABWMrcvAAAAmUlEQVR4XmNgGAUkAz4grgHiQ+gS2IAwEHcD8WEgdgJiZlRpVKAIxHMZICabAzEjqjQqUAfi1VCsxYBHMUgCZNp2BojpIFsIAn8g/gTEQQx4TMYGSPIwOoAF7QkGiAtI0swNxPlAfA6I44CYE1UaP2AF4ggGiGaQISDDiAYgZ4KcuwNdgvYA5G5xIJYkAotB9TAYAPEsInEvAL6TGRAkV7HTAAAAAElFTkSuQmCC>

[image6]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABEAAAAYCAYAAAAcYhYyAAABNUlEQVR4Xu2SLUgEURRGL6ig4B8oiKDBbQaT2iyCggbLJotR7ZaF3bIGsduMFi02EYthwWI12DQIVhGDglg8n+/dnTezhsEmeuCwO+/OvPvuN2P25+nBzaj+/4hZfMVnnCnUSjONT/iA44VaafSgNrjHsUKtNKN4iy3sTwuDuIqT8VqBzeGadXbTgy08x9508RB38RE38Ay3sIEvuOQ3Qzee4lGyZiu4bVnqlzgUaz5/LV472mAvXdD7VuJV/MDFpOZvYidZE3Xr3PiLAwuBKThnHd9xIVkT2kBNcwzglYVZNbPQ7wleWzaeo0DboTrfHXvKQtBN7MN9HIm14WgOzyM9tl7vG87jsoUchDdUAzVq08QbyzqJCt7hBR5bNtKEhXs1fu40mi/39UX00SnormLhn9/CJwtzMgG1cn+GAAAAAElFTkSuQmCC>

[image7]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAVCAYAAAB2Wd+JAAABD0lEQVR4Xu3RsUtCURTH8RMUJDYkahI4SLQK/QW1GDSKGLgJDTbrYNAQNPQntLREQ0iDo1PNQbg41JJBi9DfUBH2Pe+e+3i8MAkcHPrBR5/33Hd95zyR/8w0B7iNL07LMnrmT8ljhLN4YQEFlLGNJVtfwTr28YmarQfRTefooIoTdJFAHRd4xTuu7Z4ge7hHCoviDnhCxurR/vQ6zJG40xpIo2h8Jva3iy+MjY48G6mX8GHfP7KJNh7F3dyK1I7xhg2/oM3rEJ6xZmvaZx+H9tv3d4ek2LRzGOJS3CGaLQzEPYFGB6SD8v1V9EPfXVPcP+jIr/CAHdvk95ziBTd2HUYfR1/yanQxFq39Vp/HfAPk8S4HUPmihAAAAABJRU5ErkJggg==>

[image8]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAVCAYAAAB2Wd+JAAAA+ElEQVR4XmNgGAVUBUlAvBtdkBDgAOKtUEwSkAHiJ0DcChPgA2JPIJaF8kG0DxBLQvk8UHYoEP8G4giY4EwgbmSAmDYJiKcDcTkQXwNiRSCOB+JZQHwfiH8C8VKQRg8gTgdifSD+BMTzgVgQiPcA8Vsg1gQpYkD1H4jNkMkA0RTEAHGGDRAzMkCcCjIUxAYBDP/BAMiJV4FYBF0CClyA+BeUhgNeID7MAHE7zAZ0UAXEz4FYCVkQ5A+Qf0B+xQZg/gP5mxuIWWESyP7DBkDOB3kD5j+QejBoAOKLQCwME0ADIOeDousOEK+EssEA5BRQfBICAlA8lAAAPp8pXDz0rZIAAAAASUVORK5CYII=>

[image9]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACcAAAAVCAYAAADFEfeTAAACHUlEQVR4Xu2VzYvNURjHv0KR9/c0JGTBYiw0u7GRxSgsRkqZIgsaUqyUzYyFjWiaafIWzYqilPJWiGL+AUKSslFWs5ndjI3v5z7PyfSbkcXMdS3utz7d33m755zn+Z5zpKaaampaOmZemBXVhkZrnnmS8P1faZ35bi5WG+qpxWaPWZ9lfveatVlemN8HzU9zyKwxc7O9bmLiG+aCIioD5po5Zz6ZjeaIuWm+mTFzJ/ttVp3VYU6Y7WbUDJll5qUZMVuzX0P81q1YWKciZe1mliKtLJxv1BC/FZGmj2ZltSG124zn7z/VIvNW4aUSqarOmx9mU7Wh3sJX+AvvTaXiN3y4QHFKLylSvcr0m9Om1dxSbJJNcGE/M6dyTNE2hbc5hLvM8qzfohh/RWGrmib6bSqRalJe/Eb/M4ood5md5os5kHWc9A9mg+IleaXwNdpvHihuiTbzWbERNsqtwPgeM5j91Wve6c9PEgPY5VdzL79LJNgtB+e5YkL6EpXD2c7EWKbFLDFvsj/Cv0SWbKxWRJAsPVJsoiYq+OO/aWlSFZEqUeUa4u3dkWUu7LtmTta9V0QJTRxXxCbYzIx4m0nv6/dOWcCwwgqljQWS/n3moSIQ881TxbiTCkvgN7yGDTik0xbP2GvFS4JYCGklvSzutrmsOBxMyEtz1vSZx+ZqjjlqrmeZyB/XDIhF4JkiFgRFszU5CpSpr44t9bUxvwBriFkcwqiHGAAAAABJRU5ErkJggg==>

[image10]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAUCAYAAABSx2cSAAABJklEQVR4Xu3SvyvFYRTH8SMUEUpJITEom1ImNt2yGExKZGK3KJOSspmk/AuUhYncazMYDUaDmGQnP96f7zmP7v26Xddg86lXz33O9+l7z3nuNfvPn6cHF1jMP6gn03iJ9dfZwCOGU6EDMxiIfRsKGEdjULuDOEMRQzrYij3zNz5gB0dYiPUgDu7iGO+4jrotYwKzeLPKWSbxirXYf2s5ZRt36CurreIDc2jBadDnr2jGc/M2m6KmVfsnjKIf9+ZfUhG1oXbWy2qaU4f3zV+kcTSWLlaZijWbV+1txr4BW7gxv2FFI6R5O81fmkWtPOMKh7g0b7k3HSAjuDW/8ROMqdiOkvlF6Gfrjlq1NJv/3lqzVJu37qyYz7uErtyzH6N/SjKfe1Yzn8pJNuCHXsg1AAAAAElFTkSuQmCC>

[image11]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAWCAYAAADwza0nAAABF0lEQVR4Xu3SsUoDQRDG8QlJQLFRkKhFEMRGBNNoZyUoWNgLPkAs7CQvkiqIIBYiQcFKCL6CrUJARATRKk0KGwv9f7c7ufNAFLGwyAc/QuZ253YnMRvmT7OA63zxJ6mjly9+lwJO7Is3TmELGxiLtXKsL6KLo1hPMo1zXGAbDTxgGUto4Qrv8TPJLG5wYKG74sfqYDTW/H4aUJJfbSzhEM+Yjws8x3jEjH0ezIQeare66H5q4hnBpaUbJ3GLpi/QBHVhHSObObzg1ELDFbxixxdsWtioBtns4g3r8bsa91HzBVXcYc8LFn6ve+xbuJuiI/r9NMwkeusTziItWLN0k7Jq4ehtC/MYxP8d49liLhpYBcX8g3+cD3ogM9kFpJ9KAAAAAElFTkSuQmCC>

[image12]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABYAAAAVCAYAAABCIB6VAAABcklEQVR4Xu3SPyiFURjH8UdRhCL/UookAynFKBZklJiVgbIxUAYxmCwGFgaZZDAYTAyiCCUDC8qiFGUzIH++j+c57r2nrulu/OrTveec55z3Pee8Iv/5T5ShuCMTycV23JmJVOEu7sxCDXrRjpyUUUu6mgJUYgBv3vcdLVjCOvoxjU3kJdXU4wgrXjOPM1RjEMu4xUuYoOnBIYqRLfaAS5T6eCseMCH21ppCHGDR+8L5ppzxpNiThlGCJqfRbe7hxMdCQr/S/+F85xIlIl14x6fbQZmPdeJDogliu9Fd7YktrHWv/puSOrGtXogtPu79uhtdOJ7QJnZRM96ewj1qtaGXo5d0hXIv0HM+xYi39QHPaPG2Rs90AY9olMT57iJfCypwjVVJfAHNOBfbgUYXfBK74JBusbfr83Y4lp/j0iePib2hfi5rOEZHKPCaUbGL0fEt7KMhqpnFDTaS+r+3oh94UXJnlFCjF5UuOv+3Nf5avgAMiUVHMs40MgAAAABJRU5ErkJggg==>

[image13]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAoCAYAAABDw6Z2AAAE8UlEQVR4Xu3dXahlYxzH8b+3MhnETJi4EkqEmqZMijJMJJIo5a2Ql2kuZoZJEZ0ZzY13Ql7ShLyL5OUC6WAaMnNBGcS4IC4QSrhQXv6/nv/fevYz+5x9To19jn2+n/q3nvWsZ61n731u/v2ftdYxAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABgzlrUdvzHdvM4oO0EAACj6w+Pvz2eavp3iX7FHR7LPV7pGdHfpR6HtJ2T0DxP2/TOmS3GPL5vO4fod4+lbScAABhNN3n81fSd6/Gbx3jsH+Fx479HJ3aCx15t5wBrbfrnzAaPWvf7zAT9fc5sOwEAwGg6zWObx/yqT1W1OmHDjkjYAADA0CyOuKfq01JbnbB9ZmV5VN6O9gMeazy+9Ngjjn1rXRKh9kMej3vc4HF57F/lca/H7h4rrFxL5+hesI9jzEcel1nxQxy72OOc6Ftt3VKu5pkX/a2frYzbaOX7/WRlCVZVPX0/Ocq6Ob6zbo78bJd4nBTts+OY9EvYVIl82EqF8rHmWD9PWvlt9H03eDzisWfPiImRsAEAMIcoWZNMyFRxkzphUzLzVbRFycrCaF9n3TW0zSRC52QSqOrdux57x76ulTfrZ+KR19jX4/Noy/5VW2NznNqaezIam4lZfgfdNyf6Dvpc2s85dL0cL/osb3ncVfXJ4R6feBxZ9ela70dbCaSWh+UY6+aU82KrOZWcHevxi5Wk96Ic5JZVbTmx2Vdi+aPxAAIAAHNCJkDfWEkaro/9QQlbLqFOlrBlQqWx47GVfgmb5lZio+TorDgmeiBBVbw3beckbKn+DjnHVutN2ETVt9ebvpc8zrDeROwUK1Wy1tXN/hXN/gW245yqPtbX0v6p1b7s43Grx4NNPwAAGEGZAKmy9kFsZdgJm6iypsRM1lqpQH0a+6KxqjTpIYX6vIlMJWF71bo5ssJ2rZU5VGFTpUzLtIfFmPS89S6Jank4fwfR+eusLJHKQR63WLmmaKvkbZuVJVvJpOx+j/XRVjVP18hl56TPmcu3AABghOWrO1Rdk7HYbq6O6QGEX6P9nJVKlNrquzDaej2Ixmn7p8ed0dYxJYG6d0xtbd9rzlFb5+ieNfV9beXamYxtt5IwvWDlXi9dI89TqLI1EV03P3d+ByWFivwOx1k3hyp7muMaK1U0jTnQ4/RoP2PdE61K+MajLaq26XfUfWjvWFkWPd7jjTh+sJVK3qGxr+0THqs8vvB4zeNkK9U0fY5nY5yWnpd47Bf7aSoJKwAAwFAoEdJ9WnphrKKtNO0MOYdojqloEzZRRXCBdUulYxFJyWX9NG7OqfPqefUwwvnV/pXWu/wqJGwAAAADKGHb0nY2tLx7c7SVlL1cHZuMHnTIJVFV2DZVx5JenEvCBgAARpqezqyrXdLe7D+I7mv7sO3s43aPlR73tQcGeNHjbitLtElPlyoR/D++bBgAAGDW0oMLWuKc7nKu3vumh0Da5VAAAICRp4cI8vUlSQ8EKPRqDwAAAMywpdY9vVnTzf0b204AAAAMn/4Dg965drSVf3OVr9pQsqYX2QIAAGCG6Z6w/DdZWgbNV2io6ta+7wwAAAAzTC+33dXjNuPmfgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADA8PwD/rz0UYZVy2EAAAAASUVORK5CYII=>