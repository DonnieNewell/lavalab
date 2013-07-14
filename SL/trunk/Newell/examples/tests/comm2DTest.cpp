/* Copyright Donnie Newell 2012 */
#include <mpi.h>
#include "../comm.h"

void initializeSubDomain(SubDomain* sub_domain, DTYPE value) {
  DTYPE* buffer = sub_domain->getBuffer();
  int height = sub_domain->getLength(0);
  int width = sub_domain->getLength(1);
  for (int i = 0; i < height * width; ++i)
    buffer[i] = value;
}

void printNeighbors(SubDomain* block, int my_rank) {
  int* neighbors = block->getNeighbors();
    for (NeighborTag2D neighbor = x2DNeighborBegin; neighbor < x2DNeighborEnd; ++neighbor)
      printf("[%d]%s:%d\n", my_rank, neighborString(neighbor), neighbors[neighbor]);
}

int main(int argc, char** argv) {
  int my_rank = -1, x_offset = 0, y_offset = 0, length = 3, grid_height = 2,
          grid_width = 2;
  int id[2] = {0};
  int border[3] = {1, 1, 0};
  const int kNumNeighbors3D = 26;
  int neighbors[kNumNeighbors3D];
  for (int i = 0; i < kNumNeighbors3D; ++i) neighbors[i] = -1;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  Node node;
  SubDomain* block = NULL;
  id[0] = 0;
  id[1] = 1;
  node.setRank(my_rank);
  NeighborTag2D neighbor = x2DCorner3;
  if (0 == my_rank) {
    // initialize block 0
    neighbors[neighbor] = 1;
    
    printf("rank %d exchanges neighbor %s\n", my_rank, neighborString(neighbor));
    block = new SubDomain(id, y_offset - 1, length + 2 * border[0], x_offset - 1,
            length + 2 * border[1], grid_height, grid_width, neighbors);
  } else if (1 == my_rank) {
    // initialize block 1
    NeighborTag2D opposite_neighbor = getOppositeNeighbor2D(neighbor);
    printf("rank %d exchanges neighbor %s\n", my_rank, neighborString(opposite_neighbor));
    neighbors[opposite_neighbor] = 0;
    id[0] = 1;
    id[1] = 0;
    x_offset += length;
    block = new SubDomain(id, y_offset, length + 2 * border[0], x_offset,
            length + 2 * border[1], grid_height, grid_width, neighbors);
  }

//  printNeighbors(block, my_rank);
  initializeSubDomain(block, my_rank);
  node.addSubDomain(block);

  // perform exchange
  updateAllStaleData(&node, border);

  if (0 == my_rank) {
    printSubDomain(block);
  }
  delete block;
  block = NULL;
  MPI_Finalize();
  return 0;
}
