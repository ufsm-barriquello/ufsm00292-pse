% Leitura do log exportado pelo C
tabela = readtable('log_fsm.csv');

sucessos = sum(strcmp(tabela.Status, 'SUCESSO'));
falhas = sum(strcmp(tabela.Status, 'FALHA'));

% Criação da figura
f = figure('Visible', 'off');

subplot(1, 2, 1);
bar(categorical({'Sucesso', 'Falha'}), [sucessos, falhas], 'FaceColor', [0.2 0.6 0.8]);
ylabel('Quantidade de Pacotes');
title('Taxa de Validação da FSM');
grid on;

subplot(1, 2, 2);
bar(tabela.PacoteID, tabela.Tamanho, 'FaceColor', [0.8 0.4 0.2]);
xlabel('ID do Pacote');
ylabel('Bytes de Payload');
title('Tamanho dos Dados Recebidos');
grid on;

% Salva o gráfico como PNG na mesma pasta
exportgraphics(f, 'grafico_resultado_fsm.png', 'Resolution', 300);
fprintf('Grafico "grafico_resultado_fsm.png" gerado com sucesso!\n');
exit;