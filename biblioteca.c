/*
 * Sistema de Gerenciamento de Biblioteca
 * Disciplina: Algoritmos e Programacao
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Constantes ---
#define MAX_LIVROS 1000
#define MAX_USUARIOS 1000
#define MAX_EMPRESTIMOS 2000
#define ARQ_LIVROS "livros.txt"
#define ARQ_USUARIOS "usuarios.txt"
#define ARQ_EMPRESTIMOS "emprestimos.txt"

// --- Estruturas de Dados ---

typedef struct {
    int dia, mes, ano;
} Data;

typedef struct {
    int codigo;
    char titulo[100];
    char autor[80];
    char editora[60];
    int anoPublicacao;
    int estoque;
    int disponiveis;
    int totalEmprestado;
} Livro;

typedef struct {
    int matricula;
    char nome[100];
    char curso[50];
    char telefone[15];
    Data dataCadastro;
} Usuario;

typedef struct {
    int id;
    int matriculaUsuario;
    int codigoLivro;
    Data dataEmprestimo;
    Data dataDevolucaoPrevista;
    Data dataDevolucaoReal;
    int ativo; // 1 = Ativo, 0 = Inativo
} Emprestimo;

// --- Variáveis Globais (Banco de Dados em Memória) ---
Livro livros[MAX_LIVROS];
Usuario usuarios[MAX_USUARIOS];
Emprestimo emprestimos[MAX_EMPRESTIMOS];

int qtdLivros = 0;
int qtdUsuarios = 0;
int qtdEmprestimos = 0;

// --- Prototipos ---
int lerInteiro();
Data obterDataAtual();
Data somarDias(Data d, int dias);
int compararDatas(Data d1, Data d2);
void cabecalho(char *titulo);

// Persistência (Arquivos)
void carregarDados();
void salvarDados();

// Módulos Principais
void menuPrincipal();
void moduloLivros();
void moduloUsuarios();
void moduloEmprestimos();
void moduloRelatorios();

// Funções Específicas
void cadastrarLivro();
void cadastrarUsuario();
void realizarEmprestimo();
void realizarDevolucao();
void renovarEmprestimo();

// Buscas e Listagens
int buscarLivroPorID(int id); // Retorna índice
int buscarUsuarioPorID(int id); // Retorna índice
void pesquisarLivro();
void pesquisarUsuario();
void listarEmprestimosAtivos();
void relatorioLivrosPopulares();
void relatorioAtrasados();

// --- Implementação ---

int main() {
    setlocale(LC_ALL, "Portuguese");
    carregarDados();
    menuPrincipal();
    salvarDados();
    return 0;
}

// --- Funções Utilitárias ---



int lerInteiro() {
    char buffer[100];
    int valor;
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Remove o \n do final se existir
            buffer[strcspn(buffer, "\n")] = 0;
            
            // Tenta converter
            char *fim;
            valor = strtol(buffer, &fim, 10);
            
            // Se converteu algo e chegou ao fim da string (ou fim é espaço)
            if (fim != buffer && *fim == '\0') {
                return valor;
            }
        }
        printf("Entrada invalida. Por favor, digite um numero: ");
    }
}

Data obterDataAtual() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    Data d;
    d.dia = tm.tm_mday;
    d.mes = tm.tm_mon + 1;
    d.ano = tm.tm_year + 1900;
    return d;
}

Data somarDias(Data d, int dias) {
    struct tm t = {0};
    t.tm_year = d.ano - 1900;
    t.tm_mon = d.mes - 1;
    t.tm_mday = d.dia;
    
    // Normalizacao da data usando mktime
    t.tm_mday += dias;
    mktime(&t);
    
    Data novaData;
    novaData.dia = t.tm_mday;
    novaData.mes = t.tm_mon + 1;
    novaData.ano = t.tm_year + 1900;
    
    return novaData;
}

int compararDatas(Data d1, Data d2) {
    if (d1.ano != d2.ano) return d1.ano - d2.ano;
    if (d1.mes != d2.mes) return d1.mes - d2.mes;
    return d1.dia - d2.dia;
}

void cabecalho(char *titulo) {
    printf("\n========================================\n");
    printf("   %s\n", titulo);
    printf("========================================\n");
}

// --- Persistência de Arquivos ---

void salvarDados() {
    FILE *f;

    // Salvar Livros
    f = fopen(ARQ_LIVROS, "w");
    if (f) {
        fprintf(f, "%d\n", qtdLivros);
        for (int i = 0; i < qtdLivros; i++) {
            fprintf(f, "%d\n%s\n%s\n%s\n%d\n%d\n%d\n%d\n",
                    livros[i].codigo, livros[i].titulo, livros[i].autor,
                    livros[i].editora, livros[i].anoPublicacao,
                    livros[i].estoque, livros[i].disponiveis, livros[i].totalEmprestado);
        }
        fclose(f);
    }

    // Salvar Usuários
    f = fopen(ARQ_USUARIOS, "w");
    if (f) {
        fprintf(f, "%d\n", qtdUsuarios);
        for (int i = 0; i < qtdUsuarios; i++) {
            fprintf(f, "%d\n%s\n%s\n%s\n%d %d %d\n",
                    usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso,
                    usuarios[i].telefone, usuarios[i].dataCadastro.dia,
                    usuarios[i].dataCadastro.mes, usuarios[i].dataCadastro.ano);
        }
        fclose(f);
    }

    // Salvar Emprestimos
    f = fopen(ARQ_EMPRESTIMOS, "w");
    if (f) {
        fprintf(f, "%d\n", qtdEmprestimos);
        for (int i = 0; i < qtdEmprestimos; i++) {
            fprintf(f, "%d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                    emprestimos[i].id, emprestimos[i].matriculaUsuario, emprestimos[i].codigoLivro,
                    emprestimos[i].dataEmprestimo.dia, emprestimos[i].dataEmprestimo.mes, emprestimos[i].dataEmprestimo.ano,
                    emprestimos[i].dataDevolucaoPrevista.dia, emprestimos[i].dataDevolucaoPrevista.mes, emprestimos[i].dataDevolucaoPrevista.ano,
                    emprestimos[i].dataDevolucaoReal.dia, emprestimos[i].dataDevolucaoReal.mes, emprestimos[i].dataDevolucaoReal.ano,
                    emprestimos[i].ativo);
        }
        fclose(f);
    }
    printf("\n[Sistema] Dados salvos com sucesso (Backup Automatico).\n");
}

void carregarDados() {
    FILE *f;

    // Carregar Livros
    f = fopen(ARQ_LIVROS, "r");
    if (f) {
        fscanf(f, "%d", &qtdLivros);
        fgetc(f); // consome newline
        for (int i = 0; i < qtdLivros; i++) {
            fscanf(f, "%d", &livros[i].codigo); fgetc(f);
            fgets(livros[i].titulo, 100, f); livros[i].titulo[strcspn(livros[i].titulo, "\n")] = 0;
            fgets(livros[i].autor, 80, f); livros[i].autor[strcspn(livros[i].autor, "\n")] = 0;
            fgets(livros[i].editora, 60, f); livros[i].editora[strcspn(livros[i].editora, "\n")] = 0;
            fscanf(f, "%d", &livros[i].anoPublicacao);
            fscanf(f, "%d", &livros[i].estoque);
            fscanf(f, "%d", &livros[i].disponiveis);
            fscanf(f, "%d", &livros[i].totalEmprestado);
            fgetc(f);
        }
        fclose(f);
    }

    // Carregar Usuários
    f = fopen(ARQ_USUARIOS, "r");
    if (f) {
        fscanf(f, "%d", &qtdUsuarios);
        fgetc(f);
        for (int i = 0; i < qtdUsuarios; i++) {
            fscanf(f, "%d", &usuarios[i].matricula); fgetc(f);
            fgets(usuarios[i].nome, 100, f); usuarios[i].nome[strcspn(usuarios[i].nome, "\n")] = 0;
            fgets(usuarios[i].curso, 50, f); usuarios[i].curso[strcspn(usuarios[i].curso, "\n")] = 0;
            fgets(usuarios[i].telefone, 15, f); usuarios[i].telefone[strcspn(usuarios[i].telefone, "\n")] = 0;
            fscanf(f, "%d %d %d", &usuarios[i].dataCadastro.dia, &usuarios[i].dataCadastro.mes, &usuarios[i].dataCadastro.ano);
            fgetc(f);
        }
        fclose(f);
    }

    // Carregar Emprestimos
    f = fopen(ARQ_EMPRESTIMOS, "r");
    if (f) {
        fscanf(f, "%d", &qtdEmprestimos);
        for (int i = 0; i < qtdEmprestimos; i++) {
            fscanf(f, "%d %d %d %d %d %d %d %d %d %d %d %d %d",
                    &emprestimos[i].id, &emprestimos[i].matriculaUsuario, &emprestimos[i].codigoLivro,
                    &emprestimos[i].dataEmprestimo.dia, &emprestimos[i].dataEmprestimo.mes, &emprestimos[i].dataEmprestimo.ano,
                    &emprestimos[i].dataDevolucaoPrevista.dia, &emprestimos[i].dataDevolucaoPrevista.mes, &emprestimos[i].dataDevolucaoPrevista.ano,
                    &emprestimos[i].dataDevolucaoReal.dia, &emprestimos[i].dataDevolucaoReal.mes, &emprestimos[i].dataDevolucaoReal.ano,
                    &emprestimos[i].ativo);
        }
        fclose(f);
    }
}

// --- Menus ---

void menuPrincipal() {
    int opcao;
    do {
        cabecalho("MENU PRINCIPAL - BIBLIOTECA");
        printf("1. Gerenciar Livros\n");
        printf("2. Gerenciar Usuarios\n");
        printf("3. Emprestimos e Devolucoes\n");
        printf("4. Relatorios Avancados\n");
        printf("0. Sair e Salvar\n");
        printf("Escolha uma opcao: ");
        opcao = lerInteiro();

        switch (opcao) {
            case 1: moduloLivros(); break;
            case 2: moduloUsuarios(); break;
            case 3: moduloEmprestimos(); break;
            case 4: moduloRelatorios(); break;
            case 0: printf("Encerrando...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 0);
}

void moduloLivros() {
    int opcao;
    do {
        cabecalho("GERENCIAR LIVROS");
        printf("1. Cadastrar Novo Livro\n");
        printf("2. Pesquisar Livro\n");
        printf("0. Voltar\n");
        printf("Escolha uma opcao: ");
        opcao = lerInteiro();

        switch (opcao) {
            case 1: cadastrarLivro(); break;
            case 2: pesquisarLivro(); break;
            case 0: break;
        }
    } while (opcao != 0);
}

void moduloUsuarios() {
    int opcao;
    do {
        cabecalho("GERENCIAR USUARIOS");
        printf("1. Cadastrar Usuario\n");
        printf("2. Pesquisar Usuario\n");
        printf("0. Voltar\n");
        printf("Escolha uma opcao: ");
        opcao = lerInteiro();

        switch (opcao) {
            case 1: cadastrarUsuario(); break;
            case 2: pesquisarUsuario(); break;
            case 0: break;
        }
    } while (opcao != 0);
}

void moduloEmprestimos() {
    int opcao;
    do {
        cabecalho("EMPRESTIMOS E DEVOLUCOES");
        printf("1. Realizar Emprestimo\n");
        printf("2. Realizar Devolucao\n");
        printf("3. Listar Emprestimos Ativos\n");
        printf("4. Renovar Emprestimo\n");
        printf("0. Voltar\n");
        printf("Escolha uma opcao: ");
        opcao = lerInteiro();

        switch (opcao) {
            case 1: realizarEmprestimo(); break;
            case 2: realizarDevolucao(); break;
            case 3: listarEmprestimosAtivos(); break;
            case 4: renovarEmprestimo(); break;
            case 0: break;
        }
    } while (opcao != 0);
}

void moduloRelatorios() {
    int opcao;
    do {
        cabecalho("RELATORIOS E ESTATISTICAS");
        printf("1. Livros Mais Populares\n");
        printf("2. Usuarios com Atraso\n");
        printf("0. Voltar\n");
        printf("Escolha uma opcao: ");
        opcao = lerInteiro();

        switch (opcao) {
            case 1: relatorioLivrosPopulares(); break;
            case 2: relatorioAtrasados(); break;
            case 0: break;
        }
    } while (opcao != 0);
}

// --- Funções de Lógica de Negócios ---

int buscarLivroPorID(int id) {
    for (int i = 0; i < qtdLivros; i++) {
        if (livros[i].codigo == id) return i;
    }
    return -1;
}

int buscarUsuarioPorID(int id) {
    for (int i = 0; i < qtdUsuarios; i++) {
        if (usuarios[i].matricula == id) return i;
    }
    return -1;
}

void cadastrarLivro() {
    if (qtdLivros >= MAX_LIVROS) {
        printf("Limite de livros atingido.\n");
        return;
    }
    Livro l;
    printf("\n--- Novo Livro ---\n");
    printf("Codigo: "); 
    l.codigo = lerInteiro();
    
    if (buscarLivroPorID(l.codigo) != -1) {
        printf("Erro: Codigo ja cadastrado.\n");
        return;
    }

    printf("Titulo: "); fgets(l.titulo, 100, stdin); l.titulo[strcspn(l.titulo, "\n")] = 0;
    printf("Autor: "); fgets(l.autor, 80, stdin); l.autor[strcspn(l.autor, "\n")] = 0;
    printf("Editora: "); fgets(l.editora, 60, stdin); l.editora[strcspn(l.editora, "\n")] = 0;
    printf("Ano de Publicacao: "); l.anoPublicacao = lerInteiro();
    printf("Quantidade em Estoque: "); l.estoque = lerInteiro();
    
    l.disponiveis = l.estoque;
    l.totalEmprestado = 0;
    
    livros[qtdLivros++] = l;
    printf("Livro cadastrado com sucesso! \n");
}

void cadastrarUsuario() {
    if (qtdUsuarios >= MAX_USUARIOS) {
        printf("Limite de usuarios atingido.\n");
        return;
    }
    Usuario u;
    printf("\n--- Novo Usuario ---\n");
    printf("Matricula: "); 
    u.matricula = lerInteiro();

    if (buscarUsuarioPorID(u.matricula) != -1) {
        printf("Essa matricula ja esta cadastrada para outra pessoa.\n");
        return;
    }

    printf("Nome Completo: "); fgets(u.nome, 100, stdin); u.nome[strcspn(u.nome, "\n")] = 0;
    printf("Curso: "); fgets(u.curso, 50, stdin); u.curso[strcspn(u.curso, "\n")] = 0;
    printf("Telefone: "); fgets(u.telefone, 15, stdin); u.telefone[strcspn(u.telefone, "\n")] = 0;
    u.dataCadastro = obterDataAtual();

    usuarios[qtdUsuarios++] = u;
    printf("Usuario cadastrado com sucesso em %02d/%02d/%04d! Bem-vindo(a).\n", 
           u.dataCadastro.dia, u.dataCadastro.mes, u.dataCadastro.ano);
}

void pesquisarLivro() {
    char termo[100];
    int opcao;
    int codigoBusca;
    
    printf("Buscar por: 1. Titulo, 2. Autor, 3. Codigo: ");
    opcao = lerInteiro();
    
    if (opcao == 3) {
        printf("Digite o codigo do livro: ");
        codigoBusca = lerInteiro();
    } else {
        printf("Digite o termo de busca: ");
        fgets(termo, 100, stdin); termo[strcspn(termo, "\n")] = 0;
    }

    printf("\n--- Resultados Encontrados ---\n");
    int encontrou = 0;
    for (int i = 0; i < qtdLivros; i++) {
        if ((opcao == 1 && strstr(livros[i].titulo, termo)) || 
            (opcao == 2 && strstr(livros[i].autor, termo)) ||
            (opcao == 3 && livros[i].codigo == codigoBusca)) {
            
            char status[20];
            if (livros[i].disponiveis > 0) strcpy(status, "Disponivel");
            else strcpy(status, "Emprestado");
            
            printf("ID: %d | Titulo: %s | Autor: %s | Status: %s (%d exemplares)\n", 
                   livros[i].codigo, livros[i].titulo, livros[i].autor, status, livros[i].disponiveis);
            encontrou = 1;
        }
    }
    if (!encontrou) printf("Nenhum livro encontrado.\n");
}

void pesquisarUsuario() {
    char termo[100];
    int opcao;
    int matriculaBusca;

    printf("Buscar por: 1. Nome, 2. Matricula: ");
    opcao = lerInteiro();

    if (opcao == 2) {
        printf("Digite a matricula: ");
        matriculaBusca = lerInteiro();
    } else {
        printf("Digite nome parcial do usuario: ");
        fgets(termo, 100, stdin); termo[strcspn(termo, "\n")] = 0;
    }

    printf("\n--- Resultados ---\n");
    int encontrou = 0;
    for (int i = 0; i < qtdUsuarios; i++) {
        if ((opcao == 1 && strstr(usuarios[i].nome, termo)) ||
            (opcao == 2 && usuarios[i].matricula == matriculaBusca)) {
            printf("Matr: %d | Nome: %s | Curso: %s | Tel: %s\n", 
                   usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso, usuarios[i].telefone);
            encontrou = 1;
        }
    }
    if (!encontrou) printf("Nenhum usuario encontrado.\n");
}

void realizarEmprestimo() {
    int idLivro, idUser;
    printf("\n--- Novo Emprestimo ---\n");
    printf("Matricula do Usuario: "); 
    idUser = lerInteiro();
    
    printf("Codigo do Livro: "); 
    idLivro = lerInteiro();

    int idxUser = buscarUsuarioPorID(idUser);
    int idxLivro = buscarLivroPorID(idLivro);

    if (idxUser == -1 || idxLivro == -1) {
        printf("Usuario ou Livro nao encontrado.\n");
        return;
    }

    if (livros[idxLivro].disponiveis <= 0) {
        printf("Livro indisponivel no momento.\n");
        return;
    }

    // Verifica se usuario já tem este livro pendente
    for(int i=0; i<qtdEmprestimos; i++) {
        if(emprestimos[i].ativo && 
           emprestimos[i].matriculaUsuario == idUser && 
           emprestimos[i].codigoLivro == idLivro) {
            printf("Erro: Usuario ja possui um exemplar deste livro emprestado.\n");
            return;
        }
    }

    Emprestimo e;
    e.id = qtdEmprestimos + 1;
    e.matriculaUsuario = idUser;
    e.codigoLivro = idLivro;
    e.dataEmprestimo = obterDataAtual();
    e.dataDevolucaoPrevista = somarDias(e.dataEmprestimo, 7);
    e.ativo = 1;
    e.dataDevolucaoReal = (Data){0,0,0};

    // Atualiza estoque e estatísticas
    livros[idxLivro].disponiveis--;
    livros[idxLivro].totalEmprestado++;

    emprestimos[qtdEmprestimos++] = e;
    printf("Emprestimo #%d realizado! Devolucao prevista: %02d/%02d/%04d\n", 
           e.id, e.dataDevolucaoPrevista.dia, e.dataDevolucaoPrevista.mes, e.dataDevolucaoPrevista.ano);
}

void realizarDevolucao() {
    int idUser, idLivro;
    printf("\n--- Devolucao ---\n");
    printf("Matricula do Usuario: "); idUser = lerInteiro();
    printf("Codigo do Livro: "); idLivro = lerInteiro();

    int found = 0;
    for (int i = 0; i < qtdEmprestimos; i++) {
        if (emprestimos[i].ativo && 
            emprestimos[i].matriculaUsuario == idUser && 
            emprestimos[i].codigoLivro == idLivro) {
            
            emprestimos[i].ativo = 0;
            emprestimos[i].dataDevolucaoReal = obterDataAtual();
            
            int idxLivro = buscarLivroPorID(idLivro);
            if (idxLivro != -1) livros[idxLivro].disponiveis++;

            printf("Devolucao realizada com sucesso!\n");
            
            // Verifica atraso
            if (compararDatas(emprestimos[i].dataDevolucaoReal, emprestimos[i].dataDevolucaoPrevista) > 0) {
                printf("ATENCAO: Livro devolvido com atraso!\n");
            }
            found = 1;
            break;
        }
    }
    if (!found) printf("Nenhum emprestimo ativo encontrado para estes dados.\n");
}

void listarEmprestimosAtivos() {
    printf("\n--- Emprestimos Ativos ---\n");
    int count = 0;
    for (int i = 0; i < qtdEmprestimos; i++) {
        if (emprestimos[i].ativo) {
            int idxL = buscarLivroPorID(emprestimos[i].codigoLivro);
            int idxU = buscarUsuarioPorID(emprestimos[i].matriculaUsuario);
            
            printf("ID Emp: %d | User: %s | Livro: %s | Previsao: %02d/%02d/%04d\n",
                   emprestimos[i].id, 
                   (idxU != -1 ? usuarios[idxU].nome : "Desconhecido"),
                   (idxL != -1 ? livros[idxL].titulo : "Desconhecido"),
                   emprestimos[i].dataDevolucaoPrevista.dia,
                   emprestimos[i].dataDevolucaoPrevista.mes,
                   emprestimos[i].dataDevolucaoPrevista.ano);
            count++;
        }
    }
    if (count == 0) printf("Nenhum emprestimo ativo no momento.\n");
}

void renovarEmprestimo() {
    int idEmp;
    printf("Digite o ID do Emprestimo para renovar: ");
    idEmp = lerInteiro();
    
    int found = 0;
    for(int i=0; i<qtdEmprestimos; i++){
        if(emprestimos[i].id == idEmp && emprestimos[i].ativo){
            emprestimos[i].dataDevolucaoPrevista = somarDias(emprestimos[i].dataDevolucaoPrevista, 7);
            printf("Renovado! Nova data: %02d/%02d/%04d\n", 
                emprestimos[i].dataDevolucaoPrevista.dia,
                emprestimos[i].dataDevolucaoPrevista.mes,
                emprestimos[i].dataDevolucaoPrevista.ano);
            found = 1;
            break;
        }
    }
    if(!found) printf("Emprestimo nao encontrado ou ja encerrado.\n");
}

void relatorioLivrosPopulares() {
    printf("\n--- Top Livros Mais Emprestados ---\n");

    for(int i=0; i<qtdLivros; i++) {
        if(livros[i].totalEmprestado > 0) {
             printf("%s (Total: %d vezes)\n", livros[i].titulo, livros[i].totalEmprestado);
        }
    }
}

void relatorioAtrasados() {
    printf("\n--- Usuarios com Emprestimos em Atraso ---\n");
    Data hoje = obterDataAtual();
    int count = 0;
    
    for(int i=0; i<qtdEmprestimos; i++) {
        if(emprestimos[i].ativo) {
            if(compararDatas(hoje, emprestimos[i].dataDevolucaoPrevista) > 0) {
                int idxU = buscarUsuarioPorID(emprestimos[i].matriculaUsuario);
                int idxL = buscarLivroPorID(emprestimos[i].codigoLivro);
                
                printf("Usuario: %s | Livro: %s | Venceu em: %02d/%02d/%04d\n",
                    (idxU != -1 ? usuarios[idxU].nome : "---"),
                    (idxL != -1 ? livros[idxL].titulo : "---"),
                    emprestimos[i].dataDevolucaoPrevista.dia,
                    emprestimos[i].dataDevolucaoPrevista.mes,
                    emprestimos[i].dataDevolucaoPrevista.ano);
                count++;
            }
        }
    }
    if(count == 0) printf("Nenhum atraso registrado hoje (%02d/%02d/%04d).\n", hoje.dia, hoje.mes, hoje.ano);
}