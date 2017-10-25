#include "playerinput.h"
#include "player.h"
#include "playercubehighlight.h"
#include "playerphysics.h"
#include "playerselecteditem.h"

void PlayerInput::EnableFly(bool fly)
{
    this->noclip = fly;
    player->physics->SetKinematic(noclip);
}

void PlayerInput::SetSpeed(float speed)
{
    this->speed = speed;
}

float PlayerInput::GetSpeed()
{
    return speed;
}

void PlayerInput::Awake()
{
    this->player = gameObject->GetComponent<Player>();
    this->velocity = glm::vec3(0.f);
    this->speed = 6.f;
    this->horizontalAngle = 0.f;
    this->verticalAngle = 0.f;
    this->noclip = false;

    if(player == nullptr)
        Engine::LogError("No Player found!");
}

void PlayerInput::Update()
{
    if(player != nullptr)
    {
        Transform * t = player->camera->GetGameObject()->GetTransform();

        player->Move(velocity * Engine::DeltaTime() * (noclip ? 4.f : 1.f), noclip);

        // Clamp vertical angle
        this->verticalAngle = glm::clamp(verticalAngle, -.49f * glm::pi<float>(), .49f * glm::pi<float>());

        t->SetLocalRotation(glm::vec3(verticalAngle, horizontalAngle, 0.f));
    }
}

void PlayerInput::OnKeyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_W)
        this->velocity.z += speed;

    if(e->key() == Qt::Key_A)
        this->velocity.x += speed;

    if(e->key() == Qt::Key_S)
        this->velocity.z += -speed;

    if(e->key() == Qt::Key_D)
        this->velocity.x += -speed;

    if(e->key() == Qt::Key_Space && noclip)
        this->velocity.y += speed;

    if(e->key() == Qt::Key_Control && noclip)
        this->velocity.y += -speed;

    if(e->key() == Qt::Key_Space)
        player->Jump();

    if(e->key() == Qt::Key_F)
    {
        noclip = !noclip;
        player->physics->SetKinematic(noclip);
    }

    int selectedSlot = -1;

    if(e->key() == Qt::Key_1)
        selectedSlot = 0;

    if(e->key() == Qt::Key_2)
        selectedSlot = 1;

    if(e->key() == Qt::Key_3)
        selectedSlot = 2;

    if(e->key() == Qt::Key_4)
        selectedSlot = 3;

    if(e->key() == Qt::Key_5)
        selectedSlot = 4;

    if(e->key() == Qt::Key_6)
        selectedSlot = 5;

    if(e->key() == Qt::Key_7)
        selectedSlot = 6;

    if(e->key() == Qt::Key_8)
        selectedSlot = 7;

    if(e->key() == Qt::Key_9)
        selectedSlot = 8;

    player->SelectItemSlot(selectedSlot);
}

void PlayerInput::OnKeyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_W)
        this->velocity.z -= speed;

    if(e->key() == Qt::Key_A)
        this->velocity.x -= speed;

    if(e->key() == Qt::Key_S)
        this->velocity.z -= -speed;

    if(e->key() == Qt::Key_D)
        this->velocity.x -= -speed;

    if(e->key() == Qt::Key_Space && noclip)
        this->velocity.y -= speed;

    if(e->key() == Qt::Key_Control && noclip)
        this->velocity.y -= -speed;

    if(e->key() == Qt::Key_R)
    {
        this->horizontalAngle = 0.f;
        this->verticalAngle = 0.f;
        Update();
    }
}

void PlayerInput::OnMouseMoveEvent(QMouseEvent *e)
{
    if(!this->IsEnabled())
        return;

    // For some reason, the event's position is not the same as QCursor, and the latter works MUCH better (and smoother)
    QPoint p = Engine::GetInstance()->GetContext()->mapFromGlobal(QCursor::pos());

    glm::vec2 position = glm::vec2(p.x(), p.y());
    glm::vec2 screenCenter = glm::vec2(Engine::GetInstance()->GetContext()->width() / 2, Engine::GetInstance()->GetContext()->height() / 2);

    glm::vec2 delta = position - screenCenter;

    if(std::fabs(delta.x) < 0.001f && std::fabs(delta.y) < .001f)
        return;

    float sensibility = .1f;

    this->verticalAngle += delta.y * sensibility * Engine::DeltaTime();
    this->horizontalAngle -= delta.x * sensibility * Engine::DeltaTime();

    player->GetSelectedItemView()->AddMomentum(delta.x);
}

void PlayerInput::OnMousePressEvent(QMouseEvent *e)
{
    if(!this->IsEnabled() || e->button() != Qt::LeftButton)
        return;

    player->cubeHighlight->GetGameObject()->SetEnabled(true);
    Transform * trans = player->camera->GetGameObject()->GetTransform();

    glm::vec3 currentPosition = glm::floor(player->GetGameObject()->GetTransform()->WorldPosition());
    glm::vec3 cameraPosition = glm::floor(trans->WorldPosition());

    bool hit = false;

    player->GetSelectedItemView()->StartAnimation();

    // If it is a tool we remove blocks, if not we place blocks
    if(player->GetSelectedItem().type == Item::TOOL)
    {
        for(int i = 0; i < 15 && !hit; i++)
        {
            glm::vec3 targetPosition = trans->WorldPosition() + trans->Forward() * (.2123f * (i + 1));
            targetPosition = glm::floor(targetPosition);

            player->cubeHighlight->GetGameObject()->GetTransform()->SetLocalPosition(targetPosition + glm::vec3(.5f));

            char block = player->physics->chunkManager->GetBlock(targetPosition.x, targetPosition.y, targetPosition.z);
            hit = player->physics->chunkManager->SetBlock(targetPosition.x, targetPosition.y, targetPosition.z, -1);

            if(block != AIR && block >= 0 && hit)
                player->GetInventory()->AddItem(Item(Item::BLOCK, block, 1));
        }
    }
    else if(player->GetSelectedItem().type == Item::BLOCK && player->GetSelectedItem().quantity > 0)
    {
        char blockType = (char) player->GetSelectedItem().id;

        for(int i = 0; i < 13; i++)
        {
            glm::vec3 targetPosition = trans->WorldPosition() + trans->Forward() * (.335f * (i + 1));
            targetPosition = glm::floor(targetPosition);
            hit = player->physics->chunkManager->GetBlock(targetPosition.x, targetPosition.y, targetPosition.z) != -1;

            if(hit && glm::length(targetPosition - currentPosition) > 0.5f && glm::length(targetPosition - cameraPosition) > 0.5f)
            {
                // If we hit something, set the previous block!
                targetPosition = trans->WorldPosition() + trans->Forward() * (.335f * i);
                targetPosition = glm::floor(targetPosition);

                if(glm::length(targetPosition - currentPosition) > 0.5f && glm::length(targetPosition - cameraPosition) > 0.5f)
                {
                    hit = player->physics->chunkManager->SetBlock(targetPosition.x, targetPosition.y, targetPosition.z, blockType);
                    player->cubeHighlight->GetGameObject()->GetTransform()->SetLocalPosition(targetPosition + glm::vec3(.5f));
                    player->GetInventory()->Consume(player->GetSelectedSlot());
                }

                break;
            }
        }
    }
}

void PlayerInput::OnMouseWheelEvent(QWheelEvent *e)
{
    int slot = player->GetSelectedSlot();

    if(e->delta() >= 0)
        slot = (slot + 1 >= 9 ? 0 : slot + 1);
    else
        slot = (slot - 1 < 0 ? 8 : slot - 1);

    player->SelectItemSlot(slot);
}
