# Descrição do buffer circular
O buffer circular é uma estrutura de dados FIFO (first-in first-out) que armazena dados em um vetor e contem ponteiros para o inicio e fim dos dados. Quando acima da capacidade, sobreescreve dados anteriores. Para este exercício o buffer guarda valores integros. Quando uma operação que observa o conteudo do vetor (pop ou peek) é executada com o buffer vazio, 0 é retornado.

# Operações implementadas
* Criação do buffer com capacidade especificada.
* Inserção de dados em buffer (push).
* Remoção e retorno do primeiro dado do buffer (pop).
* Observação sem remover o primeiro dado do buffer (peek).
* Observação do número de dados armazenados no buffer.
* Deleção do buffer.