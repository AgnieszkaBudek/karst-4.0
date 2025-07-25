# This file is part of MUMPS VERSION 4.7.3
# This Version was built on Fri May 4 15:54:01 2007
#
#
#Begin orderings

# NOTE that PORD is distributed within MUMPS by default. If you would like to
# use other orderings, you need to obtain the corresponding package and modify
# the variables below accordingly.
# For example, to have Metis available within MUMPS:
#          1/ download Metis and compile it
#          2/ uncomment (suppress # in first column) lines
#             starting with LMETISDIR,  LMETIS
#          3/ add -Dmetis in line ORDERINGSF
#             ORDERINGSF  = -Dpord -Dmetis
#          4/ Compile and install MUMPS
#             make clean; make   (to clean up previous installation)
#
#          Metis is now available as an internal ordering for MUMPS.
#

#LSCOTCHDIR = $(HOME)/JY/emilio/bin/generic
#LSCOTCH   = -L$(LSCOTCHDIR) -lesmumps  -lfax -lorder -lscotch -lsymbol -ldof -lgraph -lcommon -lm

LPORDDIR = ../PORD/lib/
IPORD    = -I../PORD/include/
LPORD    = -L$(LPORDDIR) -lpord

#LMETISDIR = /local/metis/
#IMETIS    = # Metis doesn't need include files (Fortran interface avail.)
#LMETIS    = -L$(LMETISDIR) -lmetis

# The following variables will be used in the compilation process.
#ORDERINGSF = -Dscotch -Dmetis -Dpord
ORDERINGSF  = -Dpord
ORDERINGSC  = $(ORDERINGSF)
LORDERINGS = $(LMETIS) $(LPORD) $(LSCOTCH)
IORDERINGS = $(IMETIS) $(IPORD) $(ISCOTCH)

#End orderings
########################################################################
################################################################################

PLAT    =
RM = /bin/rm -f
CC =   /opt/homebrew/bin/gcc-12    #/usr/local/Cellar/gcc/9.3.0/bin/gcc-9 #gcc-6.1.0
FC =   /opt/homebrew/bin/gfortran  #/usr/local/Cellar/gcc/9.3.0/bin/gfortran-9   #gfortran-6.1.0
FL =   /opt/homebrew/bin/gfortran  #/usr/local/Cellar/gcc/9.3.0/bin/gfortran-9  #gfortran-6.1.0
AR = ar vr
#RANLIB = ranlib
RANLIB  = echo
# See point 17 in the FAQ to have more details on the compilation of mpich with gfortran
INCSEQ = -I../libseq
LIBSEQ  =  -L../libseq -lmpiseq
#LIBBLAS = -L/usr/lib/xmm/ -lf77blas -latlas
LIBBLAS = -lcblas
LIBOTHERS = -lpthread
#Preprocessor defs for calling Fortran from C (-DAdd_ or -DAdd__ or -DUPPER)
CDEFS   = -DAdd_

#Begin Optimized options
OPTF    = -O -Dintel_ -DALLOW_NON_INIT 
OPTL    = -O 
OPTC    = -O
#End Optimized options
 


INC = $(INCSEQ)
LIB = $(LIBSEQ)
LIBSEQNEEDED = libseqneeded
