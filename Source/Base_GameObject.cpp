#include "Game.h"
#include "Base_GameObject.h"

using namespace GamePlay;

Base_GameObject::Base_GameObject(float initialHealth)
	:mHP(initialHealth)
{
}

void Base_GameObject::SetHP(float hp)
{
	mHP = hp;
}

float Base_GameObject::GetHP()
{
	return mHP;
}

void Base_GameObject::ReduceHP(float deltaHP)
{
	mHP -= deltaHP;
}

BOOL Base_GameObject::IsDead()
{
	return (mHP<=0.0f);
}
