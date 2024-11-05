#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h> // Função "Sleep" no Windows
#else
#include <unistd.h>  // Função "sleep" no Linux
#endif

// Função de delay
void delay(float segundos) {
    // Windows
    #ifdef _WIN32
        Sleep((int)(segundos * 1000));
    // Linux
    #else
        usleep((int)(segundos * 1000000));
    #endif
}

// Função para Exibir a Logo
int exibir_logo() {
    // Localizar logo.txt
    FILE *arquivo = fopen("logo.txt", "r");
    if (arquivo == NULL) {
        puts("Erro ao abrir a Logo");
        return 1;
    }
    // Imprimir o Arquivo
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        printf("%s", linha);
        delay(0.5);
    }
    // Fechar o Arquivo
    fclose(arquivo);
    return 0;
}

// Função para limpar o buffer de entrada
void limpar_entrada() {
    // Windows
    #ifdef _WIN32
        fflush(stdin);
    // Linux e outros sistemas
    #else
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    #endif
}
// Estrutura para armazenar os dados de produto
typedef struct {
    int idproduto;
    char nomeproduto[50];
    float valorproduto;
    char tipovenda[20];
} Produto;

// Estrutura para armazenar os dados do carrinho
typedef struct {
    char produtocarrinho[50];
    float valortotalcarrinho;
} Carrinho;

// Função para contar quantidade de produtos no arquivo
int contar_linhas(FILE *arquivo) {
    int contador = 0;
    char linha[100];

    rewind(arquivo); 

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (strlen(linha) > 1) {
            contador++;
        }
    }
    return contador;
}

// Função para imprimir todos os produtos
void imprimir_produtos(Produto *produtos, int qtd) {
    puts(". . . . . . . . Lista de Produtos . . . . . . . .");
    for (int i = 0; i < qtd; i++) {
        printf("%d - %s Preco: %.2f por %s\n", produtos[i].idproduto, produtos[i].nomeproduto, produtos[i].valorproduto, produtos[i].tipovenda);
    }
    puts("_________________________________________________");
}

// Função para ler produtos do arquivo e preencher o array de produtos
void ler_produtos(FILE *arquivo, Produto **produtos, int qtd) {
    rewind(arquivo);
    char linha[100];
    int i = 0;

    // Aloca memória para os produtos
    *produtos = (Produto *)malloc(qtd * sizeof(Produto));
    if (*produtos == NULL) {
        puts("Erro de alocacao!");
        exit(1); 
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL && i < qtd) {
        sscanf(linha, "%49s %f %19s", (*produtos)[i].nomeproduto, &(*produtos)[i].valorproduto, (*produtos)[i].tipovenda);
        (*produtos)[i].idproduto = i + 1;
        i++;
    }
}

// Função para verificar se o produto já foi registrado
int produto_existe(Produto *produtos, int qtd, const char *nome) {
    for (int i = 0; i < qtd; i++) {
        if (strcmp(produtos[i].nomeproduto, nome) == 0) {
            return 1;  // = Produto encontrado
        }
    }
    return 0;  // = Produto não encontrado
}

// Função para adicionar novos produtos
void adicionar_produto(Produto **produtos, int *qtd) {
    FILE *arquivo = fopen("produtos.txt", "a");
    if (arquivo == NULL) {
        puts("Erro ao abrir o arquivo!");
        return;
    }

    char prodnovo[50], tiponovo[10], tipoescolha, escolhaoutro = 'S';
    float valornovo;

    do {
        printf("Digite o nome do produto: ");
        scanf(" %49[^\n]", prodnovo);

        if (produto_existe(*produtos, *qtd, prodnovo)) {
            puts("Produto ja cadastrado!");
        } 
        else {
            printf("Digite o valor do produto: ");
            scanf("%f", &valornovo);
            limpar_entrada();

            // Escolha do tipo de venda com opção
            do {
                printf("Escolha o tipo de venda (A - kg, B - unidade): ");
                scanf(" %c", &tipoescolha);
                limpar_entrada();
                if (tipoescolha == 'A') {
                    strcpy(tiponovo, "kg");
                } 
                else if (tipoescolha == 'B') {
                    strcpy(tiponovo, "unidade");
                } 
                else {
                    puts("Digite uma entrada valida!");
                    limpar_entrada();
                    delay(1);
                }
            } while (tipoescolha != 'A' && tipoescolha != 'B');

            // Adicionando o novo produto ao arquivo com uma nova linha
            fprintf(arquivo, "%s %.2f %s\n", prodnovo, valornovo, tiponovo);
            // Força a gravação
            fflush(arquivo);
            puts("Produto adicionado com sucesso!");

            // Reallocando memória do array de produtos
            Produto *temp = realloc(*produtos, (*qtd + 1) * sizeof(Produto));
            if (temp == NULL) {
                puts("Erro de alocacao de memoria!");
                fclose(arquivo);
                return;
            }
            *produtos = temp;

            // Atualizando o array de produtos
            (*produtos)[*qtd].idproduto = *qtd + 1;
            strcpy((*produtos)[*qtd].nomeproduto, prodnovo);
            (*produtos)[*qtd].valorproduto = valornovo;
            strcpy((*produtos)[*qtd].tipovenda, tiponovo);
            (*qtd)++;
        }

        // Pergunta se o usuário deseja adicionar mais produtos
        do {
            printf("Deseja adicionar outro produto? (S - Sim, N - Nao): ");
            scanf(" %c", &escolhaoutro);
            if (escolhaoutro != 'S' && escolhaoutro != 'N') {
                puts("Digite uma entrada valida!");
                limpar_entrada();
                delay(1);
            }
        } while (escolhaoutro != 'S' && escolhaoutro != 'N');

    } while (escolhaoutro == 'S');

    fclose(arquivo);
}

// Main
int main() {
    char usuario[16], senha[16], opcao;
    char caixaU[16] = "Roberto", caixaS[16] = "caixa";
    char admU[16] = "Josney", admS[16] = "adm";
    int loginCorreto = 0;

    FILE *arquivo = fopen("produtos.txt", "r");
    if (arquivo == NULL) {
        puts("Erro ao abrir o arquivo!");
        return 1;
    }

    // Contando as linhas no arquivo e alocando memória
    int qtd_linhas = contar_linhas(arquivo);
    rewind(arquivo);

    // Realiza o Array
    Produto *produtos = NULL;
    ler_produtos(arquivo, &produtos, qtd_linhas);
    fclose(arquivo);

    // Array Carrinho 
    Carrinho *carrinho = NULL;
    int num_itens = 0;

    // Logo
    exibir_logo();

    // Loop Login
    while (!loginCorreto) {
        printf("\nDigite seu Usuario: ");
        scanf("%15s", usuario);

        printf("\nDigite sua Senha: ");
        scanf("%15s", senha);

        // Tela do Caixa
        if (strcmp(usuario, caixaU) == 0 && strcmp(senha, caixaS) == 0) {
            loginCorreto = 1;
            printf("\nBem-vindo, %s!\n", usuario);

            do {
                puts("Selecione uma Opcao:");
                puts(" A. Adicionar Produto ao Carrinho");
                puts(" B. Consultar Lista de Produtos");
                puts(" C. Visualizar Carrinho");
                puts(" D. Finalizar Compra");
                puts(" E. Cancelar Compra");
                puts(" F. Sair");
                printf("Opcao: ");
                scanf(" %c", &opcao);
            
                int continuar = 'S';

                switch (opcao) {
                case 'A':
                    imprimir_produtos(produtos, qtd_linhas);

                    while (continuar == 'S') {
                        float quantidade;
                        int id = 0, j, encontrado = 0;

                        // Solicita o ID do produto desejado
                        printf("Digite o ID do produto que deseja comprar: ");
                        scanf("%d", &id);

                        // Busca o produto pelo ID
                        for (j = 0; j < qtd_linhas; j++) {
                            if (produtos[j].idproduto == id) {
                                encontrado = 1;

                                // Exibe informações do produto encontrado
                                printf("Produto encontrado: %s (%.2f por %s)\n", produtos[j].nomeproduto, produtos[j].valorproduto, produtos[j].tipovenda);
                                printf("Digite a quantidade em %s: ", produtos[j].tipovenda);
                                scanf("%f", &quantidade);

                                // Aloca espaço para o novo item no carrinho
                                carrinho = (Carrinho *)realloc(carrinho, (num_itens + 1) * sizeof(Carrinho));
                                if (carrinho == NULL) {
                                    puts("Erro de memoria!");
                                    return 1;
                                }

                                // Armazena o produto e o valor total no carrinho
                                strcpy(carrinho[num_itens].produtocarrinho, produtos[j].nomeproduto);
                                carrinho[num_itens].valortotalcarrinho = produtos[j].valorproduto * quantidade;
                                num_itens++;

                                puts("Produto adicionado ao carrinho!");
                                break;
                            }
                        }

                        if (!encontrado) {
                            puts("Produto nao encontrado!");
                        }

                        // Pergunta se o usuário deseja adicionar mais produtos
                        printf("Deseja adicionar outro produto? (S - Sim, N - Nao): ");
                        scanf(" %c", &continuar);
                    }
                    break;

                    case 'B':
                        imprimir_produtos(produtos, qtd_linhas);
                        break;

                    case 'C':
                        if (num_itens == 0) {
                            puts("O carrinho esta vazio!");
                        } 
                        else {
                            for (int i = 0; i < num_itens; i++) {
                                printf("Produto: %s | Valor Total: R$%.2f\n", carrinho[i].produtocarrinho, carrinho[i].valortotalcarrinho);
                            }
                        }
                        break;

                    case 'D':
                        if (num_itens == 0) {
                            puts("O carrinho esta vazio!");
                            delay(1);
                            break;
                        }

                        // Exibe os itens no carrinho e calcula o total
                        puts(". . . . . . . . Itens no Carrinho . . . . . . . .");
                        float valor_total = 0.0;
                        for (int i = 0; i < num_itens; i++) {
                            printf("Produto: %s | Valor Total: R$%.2f\n", carrinho[i].produtocarrinho, carrinho[i].valortotalcarrinho);
                            valor_total += carrinho[i].valortotalcarrinho;
                        }
                        printf("Valor total da compra: R$%.2f\n", valor_total);
                        puts("_________________________________________________");

                        // Pergunta se o usuário deseja finalizar a compra
                        char finalizar;
                        
                        do {
                            printf("Deseja finalizar a compra? (S - Sim, N - Nao): ");
                            scanf(" %c", &finalizar);
                            if (finalizar != 'S' && finalizar != 'N') {
                                puts("Entrada invalida!");
                            }
                        } while(finalizar != 'S' && finalizar != 'N');
                        
                        if (finalizar == 'S') {
                            printf("Compra finalizada! Valor total: R$%.2f\n", valor_total);
                            // Libera a memória alocada para o carrinho e redefine as variáveis
                            free(carrinho);
                            carrinho = NULL;
                            num_itens = 0;
                        }
                        else {
                            puts("Compra nao finalizada!");
                        }
                        break;

                    case 'E':
                        if (num_itens == 0) {
                            puts("O carrinho esta vazio!");
                            delay(1);
                            break;
                        }

                        char cancelar;

                        do {
                            printf("Tem certeza de que deseja cancelar a compra? (S - Sim, N - Nao): ");
                            scanf(" %c", &cancelar);
                            if (cancelar != 'S' && cancelar != 'N') {
                                puts("Entrada invalida!");
                            }
                        } while(cancelar != 'S' && cancelar != 'N');


                        if (cancelar == 'S') {
                            puts("Compra cancelada. Todos os itens foram removidos do carrinho");
                            // Libera a memória alocada para o carrinho e redefine as variáveis
                            free(carrinho);
                            carrinho = NULL;
                            num_itens = 0;
                        }
                        else {
                            puts("Cancelamento de compra abortado");
                        }
                        break;
                    case 'F':
                        puts("Saindo do sistema...");
                        delay(1.5);
                        break;
                    default:
                        puts("Opcao invalida!");
                        limpar_entrada();
                        delay(1);
                }
            } while (opcao != 'F');
        }

        // Tela da Administração
        else if (strcmp(usuario, admU) == 0 && strcmp(senha, admS) == 0) {
            loginCorreto = 1;
            printf("\nBem-vindo ao sistema de Administracao, %s!\n", usuario);

            do {
                puts("\nSelecione uma Opcao:");
                puts(" A. Adicionar Produto");
                puts(" B. Consultar Lista de Produtos");
                puts(" C. Sair");
                printf("Opcao: ");
                scanf(" %c", &opcao);

                switch (opcao) {
                    case 'A':
                       adicionar_produto(&produtos, &qtd_linhas);
                        break;
                    case 'B':
                        imprimir_produtos(produtos, qtd_linhas);
                        break;
                    case 'C':
                        puts("Saindo do sistema...");
                        delay(1.5);
                        break;
                    default:
                        puts("Opcao invalida!");
                        limpar_entrada();
                        delay(1);
                }
            } while (opcao != 'C');
        } 
        else {
            puts("Usuario ou senha incorretos! Tente novamente...");
            delay(2);
        }
    }

    free(produtos);
    return 0;
}