
# Problema: os *k* vizinhos mais próximos

Deseja-se encontrar para um objeto de referência *q*, quais são os *k* objetos de uma base que são mais próximos a *q*.

A distância entre pares de objeto é a Spearman Footrule Distance (SFD) usando *n* objetos de referência.

Simplificação: assumir que objetos estão em espaço métrico de L dimensões e que métrica é a distância Euclidiana.

# Entrada

Na primeira linha temos quatro números:  
  *L*, o número de dimensões de todos os objetos,
  *N*, a quantidade de objetos na base, 
  *n*, a quantidade de objetos de referência,
  *Q*, a quantidade de objetos de consulta para serem respondidas 

Nas próximas *N* linhas temos L números por linha pontos flutuante descrevendo os *N* objetos na base de busca.
Nas seguintes *n* linhas temos L números por linha pontos flutuante descrevendo os *n* objetos de referência.
Nas finais *Q* linhas temos L+1 números por linha pontos flutuante descrevendo os *q* objetos de consulta com os primeiros *L* número e um número extra que especifica a quantidade *k* de objetos mais próximos esperados para serem consultados.

# Saída

Para cada objeto de consulta *q*, o seu programa deve produzir a lista dos números de identificação do *k* objetos da base que são mais próximos a *q* de acordo com a SFD. Os números de identificação começam em 0 e terminam em *N-1*.

No caso de empates passarem de k objetos, retornar todos os objetos até os empatados (ou seja, possível haver mais de k objetos retornados).

## Exemplo de entrada

```
3 5 2 4
0 0 0
1 0 1
0 1 0
1 0 0 
3 1 3
1 1 1
0 0 1
0 -1 0
1 1 1
0 0 0
1 2 1
```

## Exemplo de saída

```
0 1 2 3
1 2 3 4
0 1 2 3
1 2 3 4 0
```

