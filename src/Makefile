MPICXX=mpicxx
CFLAGS=-std=c++0x -w
INC = -I. -I../include

all:admm dadmm dsg
admm:train.cpp ssp_admm_lr.o instanceData.o conf_util.o
	$(MPICXX) -o admm train.cpp ssp_admm_lr.o instanceData.o conf_util.o $(INC) $(CFLAGS)
ssp_admm_lr.o:ssp_admm_lr.cpp 
	$(MPICXX) -c ssp_admm_lr.cpp $(INC) $(CFLAGS)
instanceData.o:instanceData.cpp
	$(MPICXX) -c instanceData.cpp $(INC) $(CFLAGS)
conf_util.o:conf_util.cpp
	$(MPICXX) -c conf_util.cpp $(INC) $(CFLAGS)


dadmm:d_admm_lr.cpp d_admm_lr.o instanceData.o conf_util.o
	$(MPICXX) -o dadmm d_admm_train.cpp d_admm_lr.o instanceData.o conf_util.o $(INC) $(CFLAGS)
d_admm_lr.o:d_admm_lr.cpp 
	$(MPICXX) -c d_admm_lr.cpp $(INC) $(CFLAGS)

dsg:dsg.o instanceData.o conf_util.o
	$(MPICXX) -o dsg dsg_train.cpp dsg.o instanceData.o conf_util.o $(INC) $(CFLAGS)
dsg.o:dsg.cpp
	$(MPICXX) -c dsg.cpp $(INC) $(CFLAGS)

clean:
	-rm -f *.o admm dadmm dsg


