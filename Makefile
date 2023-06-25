CFLAGS = -Wall -g

src = ./src/
objs = vinapp.o data.o libLista.o saltos.o libMetadado.o opcoes.o libUtils.o
dirobjs = $(foreach obj, $(objs), $(addprefix $(src),$(objs)))

# regra default (primeira regra)
all: vina++
 
# regras de ligacao
vina++: $(dirobjs)
	gcc $(CFLAGS) $^ -o $@

# regras de compilação

# objeto principal
$(addprefix $(src), vinapp.o: vinapp.c  data.o)

# objeto com as opcoes
$(addprefix $(src), opcoes.o: opcoes.c opcoes.h data.o saltos.o libLista.o libUtils.o) 

# objeto responsavel pelo controle de dados
$(addprefix $(src), data.o: data.c data.h saltos.o libLista.o libMetadado.o libUtils.o)

$(addprefix $(src), libMetadado.o: libMetadado.c libMetadado.h libUtils.o)

$(addprefix $(src), libLista.o: libLista.c libLista.h saltos.o libUtils.o)

$(addprefix $(src), salstos.o: saltos.c saltos.h)

$(addprefix $(src), libUtils.o: libUtils.c libUtils.h)

# remove arquivos temporários
clean:
	@echo "Limpando..."
	-rm -f vina++ $(src)*~ $(src)*.o 
