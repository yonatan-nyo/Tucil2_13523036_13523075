#include "QuadTreeNode.hpp"

void QuadTreeNode::reconstructImageMatrix(vector<vector<Pixel>> &pixelMatrix, int maxDepth){

  for (int x = fromX; x < fromX+width; x++)
  {
    for (int y = fromY; y < fromY+height; y++)
    {
      pixelMatrix[y][x] = Pixel(meanPixel);
    }
  }


  if (divided && 2 <= maxDepth)
  {
    northwest->reconstructImageMatrixHelper(pixelMatrix, 2, maxDepth);
    northeast->reconstructImageMatrixHelper(pixelMatrix, 2, maxDepth);
    southwest->reconstructImageMatrixHelper(pixelMatrix, 2, maxDepth);
    southeast->reconstructImageMatrixHelper(pixelMatrix, 2, maxDepth);
  }


}

void QuadTreeNode::reconstructImageMatrixHelper(vector<vector<Pixel>> &pixelMatrix, int currentDepth, int maxDepth)
{

  if (currentDepth <= maxDepth)
  {
    for (int x = fromX; x < fromX + width; x++)
    {
      for (int y = fromY; y < fromY + height; y++)
      {
        pixelMatrix[y][x] = Pixel(meanPixel);
      }
    }
  }

  if (divided && currentDepth <= maxDepth)
  {
    northwest->reconstructImageMatrixHelper(pixelMatrix, currentDepth+1, maxDepth);
    northeast->reconstructImageMatrixHelper(pixelMatrix, currentDepth+1, maxDepth);
    southwest->reconstructImageMatrixHelper(pixelMatrix, currentDepth+1, maxDepth);
    southeast->reconstructImageMatrixHelper(pixelMatrix, currentDepth+1, maxDepth);
  }
}
