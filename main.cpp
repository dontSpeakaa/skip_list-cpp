#include "src/db.h"


int main()
{
	skip_list<int, std::string> skipList(6);
	skipList.insert_element(1, "学习");
	skipList.insert_element(3, "算法认准");
	skipList.insert_element(18, "润");
	skipList.insert_element(7, "实在卷");
	skipList.insert_element(12, "不动");
	skipList.insert_element(15, "那就赶快");
	skipList.insert_element(4, "代码随想录");
	skipList.insert_element(6, "卡哥无敌");

	std::cout << "skiplist size: " << skipList.get_size() << std::endl;

	skipList.dump_file();

	skipList.search_element(9);

	skipList.display_list();
}