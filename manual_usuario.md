# Manual do Usuário - Sistema de Biblioteca

## Visão Geral
Este sistema permite o gerenciamento completo de uma biblioteca, incluindo o cadastro de livros, usuários e o controle de empréstimos e devoluções.

## Como Compilar e Executar
Abra o arquivo `biblioteca.c` em sua IDE e compile. O programa é auto contido e utiliza apenas bibliotecas padrão.

Use um compilador GCC, por exemplo:
```bash
gcc biblioteca.c -o biblioteca
./biblioteca
```

## Funcionalidades

### 1. Gestão de Livros
*   **Cadastrar**: Permite adicionar novos livros ao acervo. É necessário informar Código, Título, Autor, Editora, Ano e Quantidade de Exemplares.
*   **Pesquisar**: Busca livros por Código, Título ou Autor.
*   **Listar**: Mostra todos os livros cadastrados e o número de exemplares disponíveis.

### 2. Gestão de Usuários
*   **Cadastrar**: Registra novos usuários. Requer Matrícula, Nome, Curso, Telefone e Data. O sistema valida se a data é válida.
*   **Pesquisar**: Localiza usuários por Matrícula ou Nome.
*   **Listar**: Exibe a lista completa de usuários.

### 3. Empréstimos e Devoluções
*   **Realizar Empréstimo**: Registra a saída de um livro para um usuário.
    *   O sistema verifica se o livro tem exemplares disponíveis.
    *   A data de devolução é calculada automaticamente para 7 dias.
*   **Realizar Devolução**: Registra a devolução do livro.
    *   Atualiza o estoque automaticamente.
    *   Avisa se houver atraso.
*   **Listar Ativos**: Mostra todos os empréstimos que ainda não foram devolvidos.

### 4. Relatórios
*   **Livros Mais Emprestados**: Exibe um ranking dos livros mais populares.
*   **Atrasos**: Lista todos os empréstimos cuja data de previsão já passou e ainda não foram devolvidos.

## Persistência de Dados
O sistema salva automaticamente todos os dados em arquivos de texto (`livros.txt`, `usuarios.txt`, `emprestimos.txt`) ao fechar o programa através da opção "0. Sair e Salvar".
Ao iniciar, o sistema carrega os dados existentes.
Também é gerado um backup automático (`.bak`) dos arquivos anteriores antes de salvar.
