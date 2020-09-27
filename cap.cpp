/*--------------------------------------
* Depertamento de Informática - UFV
* Charles Abreu Santana - Mat: 87858
* Ultima modificação: 11/09/2016
*---------------------------------------*/
#include<iostream>
#include<fstream>
#include<vector>
#include<utility>
#include <time.h>
#include <stdlib.h>

using namespace std;

typedef vector < vector<double> > Matrix;
typedef vector <int> Solution;
typedef vector <double> Lengths;
typedef pair <Lengths, Matrix> Instance;
//double const INFINITO = 9999999.0;

Instance initialize(string);
double Avaliation(Solution solucao, Lengths tamanho, Matrix fluxos);
double somaTamanhoDaLinha(vector <int> linha, Lengths tamanho);
Solution construtivo(Instance instancia);
void imprimeLeyaute(double avl, Solution solucao, ofstream &out);
Solution Swap2opt(Solution solucao, int m, int n);
Solution localSearch(Solution, Instance);
Solution completeLocalSearch(Solution solucaoInicial, Instance instancia);

int main(int argc, char* argv[]){
    if (argc != 3){
      cout << "Type:  ./cap <instList> <numb_of_runs>\n";
      exit(0);
    }
    //Abre arquivo de entrada com os caminhos da instancias
    ifstream fileNames (argv[1]);
    //Cria arquivo de saida com resultados obtidos
    ofstream resultados("resultados.csv");
    ofstream leyautes("leiautes.txt");

    string name;
    clock_t t_ini, t_fim;

    srand(time(0)); //Inicializa rand

    resultados<<"Instancia;Initial;Min_Aval;Max_Avl;TMedia_Avl;Desvio_Avl;Min_Tempo;Max_Tempo;TMedia_Tempo;Desvio_Tempo"<<endl;
    int execucoes = atoi(argv[2]); //Atoi converte de string a int
    //vector <double> vetor_avaliacoes;//Armazena todos valore de funcao objetivo
    //vector <double> vetor_tempos; //Armazena todos valores de tempo

    fileNames >> name;

    while(!fileNames.eof()){
        Instance instancia = initialize(name);


        double avl, avl2, avl3, tempo = 0, tempo_total = 0;
        Solution solucaoInicial;

        for(int i = 0; i < execucoes; i++){

            t_ini = clock();
            solucaoInicial = construtivo(instancia);
            Solution solucaoLS = localSearch(solucaoInicial, instancia);
            Solution solucaoC = completeLocalSearch(solucaoInicial, instancia);
            avl  = Avaliation(solucaoInicial, instancia.first, instancia.second);
            avl2 = Avaliation(solucaoLS,  instancia.first, instancia.second );
            avl3 = Avaliation(solucaoC,  instancia.first, instancia.second );
            t_fim = clock();

            tempo = ((double)(t_fim - t_ini) / (((double)CLOCKS_PER_SEC)));
            tempo_total += tempo;

        }

        cout << name << "\t" << tempo_total <<"\n";

        leyautes << name.substr(10, (name.size()-1)) <<"\t";
        imprimeLeyaute(avl, solucaoInicial, leyautes);

        resultados << name.substr(10, (name.size()-1)) <<",";
        resultados << avl << ",";
        resultados << avl2 << ",";
        resultados << avl3 << ",";
        resultados << "\n";

        fileNames >> name;
    }

    resultados.close();
    leyautes.close();

}

Solution localSearch(Solution solucaoInicial, Instance instancia){
    Solution melhorSolucao = solucaoInicial;
    int numero_de_facilidades = (solucaoInicial.size()-1);
    double menorValor = Avaliation(solucaoInicial,instancia.first, instancia.second);
    bool melhorou;

    do{
        melhorou = false;
        for(int i = 1; i <= numero_de_facilidades; i++ ){
            for(int j = i+1; j<= numero_de_facilidades; j++){
                Solution solucaoAux = Swap2opt(melhorSolucao, i, j);
                double valorAux = Avaliation(solucaoAux,instancia.first, instancia.second);
                if( valorAux < menorValor){
                    melhorSolucao = solucaoAux;
                    menorValor = valorAux;
                    melhorou = true;
                }
            }
        }

    } while(melhorou);

    return melhorSolucao;
}

Solution completeLocalSearch(Solution solucaoInicial, Instance instancia){
    int numero_de_facilidades = (solucaoInicial.size()-1);
    Solution melhorDeTodas = solucaoInicial;
    double melhorAval = Avaliation(solucaoInicial, instancia.first, instancia.second);

    for(int particao = 1; particao < numero_de_facilidades; particao ++){
        Solution solucaoTemporaria = solucaoInicial;
        solucaoTemporaria[0] = particao;
        //Busca Local
        solucaoTemporaria = localSearch(solucaoTemporaria, instancia);

        double avlAux = Avaliation(solucaoTemporaria, instancia.first, instancia.second);

        if(avlAux < melhorAval){
            melhorAval = avlAux;
            melhorDeTodas = solucaoTemporaria;
        }
    }

    return melhorDeTodas;
}

//Troca dois elementos de poisicao na solucao
Solution Swap2opt(Solution solucao, int m, int n){
    for(int i = 1; i < solucao.size(); i++){
        if(solucao[i] == m){
            solucao[i] = n;
        }else if(solucao[i] == n){
            solucao[i] = m;
        }else continue;
    }
    return solucao;
}

Instance initialize(string fileName){
    ifstream inFile(fileName.c_str());
    int numberOfFacilities;
    double value;
    Instance p;

    inFile >> numberOfFacilities;

    for(int i = 0; i < numberOfFacilities; i++){
        inFile >> value;
        p.first.push_back(value);
    }

    for(int i =0; i < numberOfFacilities; i++){
        vector <double> linha;
        for(int j = 0; j< numberOfFacilities; j++){
            inFile >> value;
            linha.push_back(value);
        }
        p.second.push_back(linha);
    }

    return p;
}

double Avaliation(Solution solucao, Lengths tamanho, Matrix fluxos ){
    double total = 0.0, total2 = 0.0;
    int numero_de_facilidades = (solucao.size() -1);
    int particao = solucao[0];
    vector <double> coordenada (numero_de_facilidades, 0.0) ;
    //Pega coordenadas das facilidades da primeira linha
    for(int i = 1; i <= particao; i++){
        coordenada[(solucao[i]-1)] = total + tamanho[(solucao[i]-1)]/2;
        total += tamanho[(solucao[i]-1)];
    }
    total = 0;
    //Pega coordenadas das facilidades  da segunda linha
    for(int j = numero_de_facilidades; j > particao; j--){
        coordenada[solucao[j]-1] = total + tamanho[solucao[j]-1]/2;
        total += tamanho[solucao[j]-1];
    }

    total = 0;
    //Faz a soma ponderadadas distancias entre as facilidades
    for(int i = 0; i < numero_de_facilidades; i++){
        for(int j = i; j < numero_de_facilidades; j++){
            total2 = coordenada[i]-coordenada[j];
            if(total2 < 0){
                total += fluxos[i][j] * (total2 * (-1));
            }else{
                total += fluxos[i][j] * total2;
            }

        }
    }
    return total;
}

Solution construtivo(Instance instancia){
    int numero_de_facilidades = instancia.first.size();
    Solution solucao((numero_de_facilidades + 1), 0);
    vector <int> linha1;
    vector <int> linha2;
    vector <int> estaNaSolucao (numero_de_facilidades, 0);
    double maximo = -1;
    int atual = 0, aux = 0;

    Matrix fluxos_pesos;

    //Cria matriz com as relacoes trafego/tamanho
    for(int i =0; i< numero_de_facilidades; i++){
        vector <double> linha;
        for(int j =0; j < numero_de_facilidades; j++){
            linha.push_back(instancia.second[i][j]/instancia.first[i]);
        }
        fluxos_pesos.push_back(linha);
    }


    //Obtem elemento de maior trafego/tamanho
    for(int i = 0; i < numero_de_facilidades; i++){
        for(int j = 0; j < numero_de_facilidades; j++){
            if(i == j) continue;
            if(fluxos_pesos[i][j] > maximo){
                maximo = fluxos_pesos[i][j];
                atual = i;
            }
        }
    }
    maximo = -1;
    estaNaSolucao[atual] = 1;

    //Coloca o primeiro ponto na solucao
    linha1.push_back(atual + 1);

    //Constroi solução colocanco facilidades de maior trafego juntas
    //As linhas do corredor são balanceadas
    for(int i = 0; i<(numero_de_facilidades-1); i++){ //-1 porque ja colocou o primeiro elemento
        if(somaTamanhoDaLinha(linha1, instancia.first) >= somaTamanhoDaLinha(linha2, instancia.first)){//Coloca elemento na linha 2
            //Pega ultimo elemento da linha
            if(linha2.empty()){
                atual = (linha1[0]-1);
            } else{
                atual = ((linha2[linha2.size()-1]) - 1);
            }
            //Escolhe adjascente com maior trafego/tamanho
            for(int j = 0;  j < numero_de_facilidades; j++){
                if(atual==j) continue;
                else if(((fluxos_pesos[atual][j] > maximo) || (maximo == -1)) && (estaNaSolucao[j] == 0)){
                    aux = j;
                    maximo = fluxos_pesos[atual][j];
                }
            }

            maximo = -1;
            estaNaSolucao[aux] = 1;
            linha2.push_back(aux+1);

        } else {//Insere elemento na linha 1
            atual = ((linha1[linha1.size()-1]) - 1);
            //Escolhe adjascente com maior trafego/tamanho
            for(int j = 0;  j < numero_de_facilidades; j++){
                if(atual==j) continue;
                else if(((fluxos_pesos[atual][j] > maximo) || (maximo == -1)) && (estaNaSolucao[j] == 0)){
                    aux = j;
                    maximo = fluxos_pesos[atual][j];
                }
            }

            maximo = -1;
            estaNaSolucao[aux] = 1;
            linha1.push_back(aux+1);
        }
    }

    // Cria a solução de acordo com a representação (vetor de int)
    int particao = linha1.size();
    solucao[0] = particao;
    //Coloca primeira linha na solucao
    for(int i = 1; i <= particao; i++){
        solucao[i] = linha1[i-1];
    }
    int i = 0;
    //Coloca segunda linha (inversa) na solucao
    for(int j = numero_de_facilidades; j > particao; j--){
        solucao[j] = linha2[i];
        i++;
    }

    return solucao;

}

//Retorna a soma de um vector de double
double somaTamanhoDaLinha(vector <int> linha, Lengths tamanho){
    if(linha.empty()) return 0.0;
    double soma = 0.0;
    for(int i=0; i < linha.size(); i++){
        soma += tamanho[(linha[i] - 1)];
    }
    return soma;
}

void imprimeLeyaute(double avl, Solution solucao, ofstream &out){
    int particao = solucao[0];
    int numero_de_facilidades = (solucao.size()-1);
    out << avl <<"\n\n";
    //Imprime linha 1
    for(int i=0; i<=particao; i++){
        out << solucao[i];
        if(i != particao) out <<"-";
    }
    out <<"\n";
    //Imprime linha 2
    for(int i = numero_de_facilidades; i > particao; i--){
        out << solucao[i];
        if(i != (particao+1)) out <<"-";
    }
    out << "\n\n";
}
