#include "keyboard.h"

KeyBoard::KeyBoard(std::string path, bool debug) : RemoteProduct(path, debug) {}

KeyBoard::~KeyBoard() {}

int KeyBoard::Init()
{
    return 0;
}

int KeyBoard::GetKeyBoard()
{
    return 0;
}

bool KeyBoard::Request(struct RemoteState &data)
{
    return false;
}