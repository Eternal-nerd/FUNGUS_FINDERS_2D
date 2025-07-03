#include "player.h"

/*
-----~~~~~=====<<<<<{_INITIALIZATION_}>>>>>=====~~~~~-----
*/
void Player::init(GameState& gameState, glm::vec2 position, glm::vec2 sizePercent, int textureIndex) {
	log(name_ + __func__, "init player");
	
	gameState_ = &gameState;

	position_ = position;
	sizePercent_ = sizePercent;
	textureIndex_ = textureIndex;
    textureOffset_ = 0.25f;

	// init tex position
    texturePosition_ = {0.f, 0.f};
    textureMirrored_ = false;

    // initialize vertices
	float xOffset = (sizePercent_.x * 2) * gameState_->spriteScale;
	float yOffset = (sizePercent_.y * 2) * gameState_->spriteScale;

	// top left
	vertices_[0].pos = { position_.x, position_.y };
	vertices_[0].texCoord = { 0.f, 0.f };
	vertices_[0].texIndex = textureIndex_;
	vertices_[0].interaction = 0;

	// bottom left
	vertices_[1].pos = { position_.x, position_.y + yOffset };
	vertices_[1].texCoord = { 0.f, 0.25f };
	vertices_[1].texIndex = textureIndex_;
	vertices_[1].interaction = 0;

	// top right
	vertices_[2].pos = { position_.x + xOffset, position_.y };
	vertices_[2].texCoord = { 0.25f, 0.f };
	vertices_[2].texIndex = textureIndex_;
	vertices_[2].interaction = 0;

	// bottom right
	vertices_[3].pos = { position_.x + xOffset, position_.y + yOffset };
	vertices_[3].texCoord = { 0.25f, 0.25f };
	vertices_[3].texIndex = textureIndex_;
	vertices_[3].interaction = 0;
}

/*
-----~~~~~=====<<<<<{_UPDATES_}>>>>>=====~~~~~-----
*/
void Player::update() {
    // decceleration x
	if (noX_ && !stoppedX_) {
        if (velocity_.x < 0.f) {
            // check if stop
            if (velocity_.x + (PLAYER_DECELERATION * gameState_->simulationTimeDelta) >= 0) {
                stoppedX_ = true;
            }
            else {
                 acceleration_.x = PLAYER_DECELERATION;
            }
        }
        if (velocity_.x > 0.f) {
            // check if stop
            if (velocity_.x - (PLAYER_DECELERATION * gameState_->simulationTimeDelta) <= 0) {
                stoppedX_ = true;
            }
            else {
                 acceleration_.x = -PLAYER_DECELERATION;
            }
        }
    }
    
    // on x stop 
    if (stoppedX_) {
        acceleration_.x = 0.f;
        velocity_.x = 0.f;
    }
    
    // deceleration y
    if (noY_ && !stoppedY_) {
        if (velocity_.y < 0.f) {
            // check if stop
            if (velocity_.y + (PLAYER_DECELERATION * gameState_->simulationTimeDelta) >= 0) {
                stoppedY_ = true;
            }
            else {
                 acceleration_.y = PLAYER_DECELERATION;
            }
        }
        if (velocity_.y > 0.f) {
            // check if stop
            if (velocity_.y - (PLAYER_DECELERATION * gameState_->simulationTimeDelta) <= 0) {
                stoppedY_ = true;
            }
            else {
                 acceleration_.y = -PLAYER_DECELERATION;
            }
        }
    }
    
    // on y stop 
    if (stoppedY_) {
        acceleration_.y = 0.f;
        velocity_.y = 0.f;
    }

    if (noX_ && noY_) {
        textureStage_ = 0;
    }

    // calculate velocity
    velocity_ += glm::vec2(acceleration_.x * gameState_->simulationTimeDelta, acceleration_.y * gameState_->simulationTimeDelta);

    // max velocity changes if sprinting
    float maxVelocity = (sprinting_) ? MAX_PLAYER_VELOCITY * PLAYER_SPRINT_MULTIPLIER : MAX_PLAYER_VELOCITY;

    // limit velocity
    if (velocity_.y > maxVelocity) {
		velocity_.y = maxVelocity;
    }
	if (velocity_.y < -maxVelocity) {
		velocity_.y = -maxVelocity;
	}

   	if (velocity_.x > maxVelocity) {
		velocity_.x = maxVelocity;
    }
	if (velocity_.x < -maxVelocity) {
		velocity_.x = -maxVelocity;
	}

	// update position based on velocity
	position_ += glm::vec2(velocity_.x * gameState_->simulationTimeDelta, velocity_.y * gameState_->simulationTimeDelta);

    // add position to animation counter
    animationDistance_ += (abs(velocity_.x) >= abs(velocity_.y))  ? abs(velocity_.x * gameState_->simulationTimeDelta) : abs(velocity_.y * gameState_->simulationTimeDelta);
    if (animationDistance_ > PLAYER_ANIMATION_RESET) {
        if (textureStage_ == 3) {
            textureStage_ = 0;
        }
        else {
            textureStage_++;
        }
        animationDistance_ = 0.f;
    }

    // edge of screen collision x
    if (position_.x <= -1.f || position_.x >= (1.f - (sizePercent_.x * 2) * gameState_->spriteScale)) { 
        // reset velocity and acceleration
        velocity_.x = 0.f;
        acceleration_.x = 0.f;
       
        // limit position
        if (position_.x < 0.f) {
            position_.x = -1.f;
        }
        else {
            position_.x = 1.f - (sizePercent_.x * 2) * gameState_->spriteScale;
        }
    }

    // edge of screen collision y
    if (position_.y <= -1.f || position_.y >= (1.f - (sizePercent_.y * 2) * gameState_->spriteScale)) { 
        // reset velocity and acceleration
        velocity_.y = 0.f;
        acceleration_.y = 0.f;

        // limit position
        if (position_.y < 0.f) {
            position_.y = -1.f;
		}
        else {
            position_.y = 1.f - (sizePercent_.y * 2) * gameState_->spriteScale;
		}
    }

    scale();

	gameState_->needTriangleRemap = true;

    // update determines the "stage" of animation based on amount moved 
    updateTextureCoords();
}

void Player::updateTextureCoords() {
    textureMirrored_ = false;

    switch (movementState_) {
    case FORWARD:
        switch (textureStage_) {
        case 0:
        case 2:
            texturePosition_ = { 0.f, 0.75f };
            break;
        case 1:
            texturePosition_ = { 0.25f, 0.75f };
            break;
        case 3:
            texturePosition_ = { 0.5f, 0.75f };
            break;
        }
        break;
    case FORWARD_LEFT:
        textureMirrored_ = true;
    case FORWARD_RIGHT:
        switch (textureStage_) {
        case 0:
        case 2:
            texturePosition_ = { 0.75f, 0.f };
            break;
        case 1:
            texturePosition_ = { 0.75f, 0.25f };
            break;
        case 3:
            texturePosition_ = { 0.75f, 0.5f };
            break;
        }
        break;
    case DOWN:
        switch (textureStage_) {
        case 0:
        case 2:
            texturePosition_ = { 0.f, 0.f };
            break;
        case 1:
            texturePosition_ = { 0.f, 0.25f };
            break;
        case 3:
            texturePosition_ = { 0.f, 0.5f };
            break;
        }
        break;
    case DOWN_LEFT:
        textureMirrored_ = true;
    case DOWN_RIGHT:
        switch (textureStage_) {
        case 0:
        case 2:
            texturePosition_ = { 0.25f, 0.f };
            break;
        case 1:
            texturePosition_ = { 0.25f, 0.25f };
            break;
        case 3:
            texturePosition_ = { 0.25f, 0.5f };
            break;
        }
        break;
    case LEFT:
        textureMirrored_ = true;
    case RIGHT:
        switch (textureStage_) {
        case 0:
        case 2:
            texturePosition_ = {0.5f, 0.f};
            break;
        case 1:
            texturePosition_ = {0.5f, 0.25f};
            break;
        case 3:
            texturePosition_ = {0.5f, 0.5f};
            break;
        }
        break;
    case STOPPED:
        texturePosition_ = {0.f, 0.f};
        break; 
    }
    
    // TODO - fix this
    // update vertices
    if (textureMirrored_) {
	    vertices_[0].texCoord = { texturePosition_.x + textureOffset_, texturePosition_.y };
	    vertices_[1].texCoord = { texturePosition_.x + textureOffset_, texturePosition_.y + textureOffset_ };
	    vertices_[2].texCoord = { texturePosition_.x, texturePosition_.y };
	    vertices_[3].texCoord = { texturePosition_.x, texturePosition_.y + textureOffset_ };
    }
    else {
        // top left
	    vertices_[0].texCoord = { texturePosition_.x, texturePosition_.y };
	    // bottom left
	    vertices_[1].texCoord = { texturePosition_.x, texturePosition_.y + textureOffset_ };
	    // top right
	    vertices_[2].texCoord = { texturePosition_.x + textureOffset_, texturePosition_.y };
	    // bottom right
	    vertices_[3].texCoord = { texturePosition_.x + textureOffset_, texturePosition_.y + textureOffset_ };
    }
}

int Player::map(Vertex* mapped) {
	for (int i = 0; i < 4; i++) {
		mapped->pos.x = vertices_[i].pos.x; // position x
		mapped->pos.y = vertices_[i].pos.y; // position y
		mapped->texCoord.x = vertices_[i].texCoord.x; // tex coord x
		mapped->texCoord.y = vertices_[i].texCoord.y; // tex coord y
		mapped->texIndex = vertices_[i].texIndex; // tex index
		mapped->interaction = vertices_[i].interaction; // for checking hover
		mapped++;
	}
	return 4;
}


void Player::scale() {
	// calculate x and y offsets
	float xOffset = (sizePercent_.x * 2) * gameState_->spriteScale;
	float yOffset = (sizePercent_.y * 2) * gameState_->spriteScale;

	// update position if needed?
	vertices_[0].pos = position_;

	// bottom left
	vertices_[1].pos = { position_.x, position_.y + yOffset };

	// top right
	vertices_[2].pos = { position_.x + xOffset, position_.y };

	// bottom right
	vertices_[3].pos = { position_.x + xOffset, position_.y + yOffset };
}

void Player::onKey() {
    sprinting_ = gameState_->keys.shift;

	if (gameState_->keys.w && !gameState_->keys.s) {
		acceleration_.y = -PLAYER_ACCELERATION;
		noY_ = false;
        stoppedY_ = false;
        if (gameState_->keys.d) {
            movementState_ = FORWARD_RIGHT;
        }
        else if (gameState_->keys.a) {
            movementState_ = FORWARD_LEFT;
        }
        else {
            movementState_ = FORWARD;
        }
	}
	if (gameState_->keys.s && !gameState_->keys.w) {
		acceleration_.y = PLAYER_ACCELERATION;
		noY_ = false;
        stoppedY_ = false;
	    if (gameState_->keys.d) {
            movementState_ = DOWN_RIGHT;
        }
        else if (gameState_->keys.a) {
            movementState_ = DOWN_LEFT;
        }
        else {
            movementState_ = DOWN;
        }
    }
    if (!gameState_->keys.w && !gameState_->keys.s) {
		noY_ = true;
	}
    if (gameState_->keys.w && gameState_->keys.s) {
        stoppedY_ = true;
    }
	
    if (gameState_->keys.d && !gameState_->keys.a) {
		acceleration_.x = PLAYER_ACCELERATION;
		noX_ = false;
        stoppedX_ = false;
        if (!gameState_->keys.w && !gameState_->keys.s) {
            movementState_ = RIGHT;
        }
	}
	if (gameState_->keys.a && !gameState_->keys.d) {
		acceleration_.x = -PLAYER_ACCELERATION;
		noX_ = false;
        stoppedX_ = false;
        if (!gameState_->keys.w && !gameState_->keys.s) {
            movementState_ = LEFT;
        }
	}
	if (!gameState_->keys.a && !gameState_->keys.d) {
		noX_ = true;

	}
    if (gameState_->keys.a && gameState_->keys.d) {
        stoppedX_ = true;
    }
}

/*
-----~~~~~=====<<<<<{_CLEANUP_}>>>>>=====~~~~~-----
*/
void Player::cleanup() {

}
