#ifndef _INDIRECT_BLOCK_H_
#define _INDIRECT_BLOCK_H_

class IndirectBlock {
public:
  IndirectBlock() {};
  ~IndirectBlock() {};

  void fetchFrom(int start_sector);
  void writeBack(int start_sector);  

  int* getFragments() { return fragments; };
  void setFragments(int *new_fragments, int size) {
      int end = size > 128 ? 128 : size;
      for (int i = 0;i < 128; ++i)
	  fragments[i] = 0;
      for (int i = 0;i < end; ++i)
	  fragments[i] = new_fragments[i];
  };
private:
  int fragments[128];
};

#endif
