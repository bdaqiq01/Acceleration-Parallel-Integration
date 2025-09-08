include_dirs = -I/opt/intel/compilers_and_libraries_2020.0.166/linux/mpi/intel64/include/
lib_dirs = -L/opt/intel/compilers_and_libraries_2020.0.166/linux/mpi/intel64/lib/debug -L/opt/intel/compilers_and_libraries_2020.0.166/linux/mpi/intel64/lib

# compilers
cxx = g++
mpicxx = mpicxx

# compiler flags
cxxflags = -O2 -std=c++11
cflags = -g -Wall $(include_dirs)

# target executable names and sources
target_seq = integration_seq
src_seq = integration_seq.cpp

target_mpi = integration_mpi
src_mpi = MPI_OMP_FP.cpp

# build both targets by default
all: $(target_seq) $(target_mpi)

# rule for sequential (non-mpi) target
$(target_seq): $(src_seq)
	$(cxx) $(cxxflags) -o $@ $^ $(include_dirs) $(lib_dirs)

# rule for mpi-based target with openmp support
$(target_mpi): $(src_mpi)
	$(mpicxx) $(cflags) -fopenmp -o $@ $^ $(lib_dirs)

# run sequential version
run: $(target_seq)
	./$(target_seq) $(args)

# run mpi version (example: make run_mpi nprocs=4)
run_mpi: $(target_mpi)
	mpirun -np $(nprocs) ./$(target_mpi)

# clean up build artifacts
clean:
	rm -f $(target_seq) $(target_mpi) *.o *~
