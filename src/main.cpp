#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <iostream>

using namespace geode::prelude;

float direction_modifier_left = 0.0f;
float direction_modifier_right = 0.0f;
float platformer_x_position = 0.0f;
float acceleration = 0.0f;

bool isGravityUp = false;

bool moving = false;
bool flip = false;

float timeLeft = 0.0f;

CCMenuItemSprite* button;
CCMenuItemSprite* buttonActivated;
CCSprite* idleSprite;
CCSprite* pressedSprite;

void executePlatformer(PlayerObject* player, float deltaTime) {
	flip = (direction_modifier_right - direction_modifier_left) == 0 ? flip : (
		(direction_modifier_right - direction_modifier_left) > 0 ? false : true
	);

	moving = (direction_modifier_right - direction_modifier_left) != 0;
	timeLeft += deltaTime;

	platformer_x_position += (player->m_playerSpeed * deltaTime * (direction_modifier_right - direction_modifier_left) * 351.5f);
	player->m_position.x = platformer_x_position;

	button->setVisible(!moving);
	buttonActivated->setVisible(moving);

	buttonActivated->setScaleX((flip ? -1 : 1) * 1.375f);


	moving && player->m_isOnGround ? player->m_playerGroundParticles->resumeSystem() : player->m_playerGroundParticles->stopSystem();
	moving ? button->setColor({ 200, 200, 200 }) : button->setColor({ 255, 255, 255 });
	

	player->setScaleX(
		(flip ? -1 : 1) * abs(player->getScaleX())
	);
}

class $modify(GameObject) {

	void activateObject() {
		this->m_hasBeenActivated = false;
		this->m_hasBeenActivatedP2 = false;
		GameObject::activateObject();
	}
};

class $modify(UILayer) {

	bool init() {
		auto ret = UILayer::init();

		idleSprite = CCSprite::create("control.png"_spr);
		pressedSprite = CCSprite::create("control-pressed.png"_spr);

		button = CCMenuItemSprite::create(idleSprite,
			idleSprite,
			idleSprite
		);
		buttonActivated = CCMenuItemSprite::create(pressedSprite,
			pressedSprite,
			pressedSprite
		);

		CCMenu* menu = CCMenu::create();
		menu->addChild(button);
		menu->addChild(buttonActivated);
		this->addChild(menu);

		menu->setPosition({ 0, 0 });
		button->setAnchorPoint({ 0.5, 0.5 });
		button->setPosition({ 59, 27 });
		button->setScale(1.375f);

		buttonActivated->setAnchorPoint({ 0.5, 0.5 });
		buttonActivated->setPosition({ 59, 27 });
		buttonActivated->setScale(1.375f);

		return ret;
	}

	void keyDown(enumKeyCodes key) {
		log::debug("Pressed the key: {}", key);

		if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_A) {
			direction_modifier_left = 1.0f;
		}
		if (key == enumKeyCodes::KEY_Right || key == enumKeyCodes::KEY_D) {
			direction_modifier_right = 1.0f;
		}


		UILayer::keyDown(key);
	}

	void keyUp(enumKeyCodes key) {
		log::debug("Released the key: {}", key);

		if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_A) {
			direction_modifier_left = 0.0f;
		}
		if (key == enumKeyCodes::KEY_Right || key == enumKeyCodes::KEY_D) {
			direction_modifier_right = 0.0f;
		}


		UILayer::keyUp(key);
	}
};


class $modify(PlayLayer) {

	bool init(GJGameLevel* level) {
		auto ret = PlayLayer::init(level);

		log::info("Disabling the VSync because of genious game mechanics");
		AppDelegate::sharedApplication()->toggleVerticalSync(false);

		return ret;
	}

	void resetLevel() {
		platformer_x_position = 0.0f;
		flip = false;
		isGravityUp = false;
		return PlayLayer::resetLevel();
	}

	void flipGravity(PlayerObject* po, bool smth1, bool smth2) {
		log::info("smth1: {}", smth1);
		log::info("smth2: {}", smth2);
		isGravityUp = smth1;
		PlayLayer::flipGravity(po, smth1, smth2);
	}

	void playGravityEffect(bool smth) {
		log::info("grav: {}", smth);
		isGravityUp = smth;
		PlayLayer::playGravityEffect(smth);
	}

	void update(float deltaTime) {
		if (!this->m_isDead) {
			executePlatformer(this->m_player1, deltaTime);
		}
			
			
		return PlayLayer::update(deltaTime);
	}
};

class $modify(PlatformerPlayerObject, PlayerObject) {

	void update(float dt) {

		if (!(this->m_isShip || this->m_isBall || this->m_isDart || this->m_isSpider || this->m_isRobot))
			this->setRotation(!isGravityUp ? 0.0 : -180);
		return PlayerObject::update(dt);
	}

	void playStitchAnimation() {
		float vsize_mod = this->m_vehicleSize == 1.0f ? 1.0f : this->m_vehicleSize;

		CCArray* actions_array = CCArray::create();
		actions_array->addObject(CCScaleTo::create(0.1f, 0.6f * (flip ? -1 : 1) * vsize_mod, 1.3f * vsize_mod));
		actions_array->addObject(CCScaleTo::create(0.1f, 1.0f * (flip ? -1 : 1) * vsize_mod, 1.0f * vsize_mod));

		runAction(CCSequence::create(actions_array));
	}

	void pushButton(int btn) {
		if (!(this->m_isShip || this->m_isBall || this->m_isDart || this->m_isSpider || this->m_isRobot || (this->m_vehicleSize != 1.0)))
			PlatformerPlayerObject::playStitchAnimation();
		return PlayerObject::pushButton(btn);
	}
};