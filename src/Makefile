MPICXX=mpicxx
CFLAGS=-std=c++0x -w
INC = -I. -I../include

all:admm dsg radmm
admm:train.cpp ssp_admm_lr.o instanceData.o conf_util.o
	$(MPICXX) -o admm train.cpp ssp_admm_lr.o instanceData.o conf_util.o $(INC) $(CFLAGS)
ssp_admm_lr.o:ssp_admm_lr.cpp 
	$(MPICXX) -c ssp_admm_lr.cpp $(INC) $(CFLAGS)
instanceData.o:instanceData.cpp
	$(MPICXX) -c instanceData.cpp $(INC) $(CFLAGS)
conf_util.o:conf_util.cpp
	$(MPICXX) -c conf_util.cpp $(INC) $(CFLAGS)


dsg:dsg.o instanceData.o conf_util.o
	$(MPICXX) -o dsg dsg_train.cpp dsg.o instanceData.o conf_util.o $(INC) $(CFLAGS)
dsg.o:dsg.cpp
	$(MPICXX) -c dsg.cpp $(INC) $(CFLAGS)

radmm: randomized_admm.o instanceData.o conf_util.o
	$(MPICXX) -o radmm randomized_train.cpp randomized_admm.o instanceData.o conf_util.o $(INC) $(CFLAGS)
randomized_admm.o: randomized_admm.cpp
	$(MPICXX) -c randomized_admm.cpp $(INC) $(CFLAGS)

clean:
	-rm -f *.o admm  dsg


