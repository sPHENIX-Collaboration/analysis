#include "Tower.h"

Tower::~Tower() = default;

void Tower::CopyTo(Tower* tower)
{
    tower->set_px(Px);
    tower->set_py(Py);
    tower->set_pz(Pz);
    tower->set_e(E);
    tower->set_calo(calo);

    return;
}