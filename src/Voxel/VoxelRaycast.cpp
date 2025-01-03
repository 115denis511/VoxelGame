#include "VoxelRaycast.h"

bool engine::VoxelRaycast::raycastVoxel(
    ChunkGrid &grid, const ChunkGridBounds &bounds, 
    const glm::ivec3 &voxelPosition, const glm::vec3 &direction, float maxDistance, glm::ivec3 &hitPosition, glm::ivec3 &hitFace
) {
    // Сделано на основе следующих материалов:
    // https://gamedev.stackexchange.com/questions/47362/cast-ray-to-select-block-in-voxel-game
    // https://gedge.ca/blog/2014-01-20-voxel-iteration/
    // https://github.com/thegedge/voxel_iterator/blob/main/voxel_iterator.cpp

    // если начальная позиция вне чанков или направление равно нулю, то возвращаем false
    if (!bounds.isPositionInbounds(voxelPosition) || (direction.x == 0 && direction.y == 0 && direction.z == 0)) return false;

    hitPosition = voxelPosition;
    glm::vec3 next(intbound(hitPosition.x, direction.x), intbound(hitPosition.y, direction.y), intbound(hitPosition.z, direction.z));
    glm::ivec3 step(signum(direction.x), signum(direction.y), signum(direction.z));
    glm::vec3 delta(step.x / direction.x, step.y / direction.y, step.z / direction.z);
    maxDistance /= std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

    bool first = true;
    while (bounds.isPositionInbounds(hitPosition)) {
        if (!first && grid.isPositionHasSolidVoxel(hitPosition, bounds)) return true;
        first = false;

        if(next.x < next.y) {
            if(next.x < next.z) {
                if (next.x > maxDistance) break;
                hitPosition.x += step.x;
                next.x += delta.x;
                hitFace = glm::ivec3(-step.x, 0, 0);
            } else {
                if (next.z > maxDistance) break;
                hitPosition.z += step.z;
                next.z += delta.z;
                hitFace = glm::ivec3(0, 0, -step.z);
            }
        } else if(next.y < next.z) {
            if (next.y > maxDistance) break;
            hitPosition.y += step.y;
            next.y += delta.y;
            hitFace = glm::ivec3(0, -step.y, 0);
        } else {
            if (next.z > maxDistance) break;
            hitPosition.z += step.z;
            next.z += delta.z;
            hitFace = glm::ivec3(0, 0, -step.z);
        }
    }

    return false;
}

int engine::VoxelRaycast::signum(float x)
{
    return x > 0 ? 1 : x < 0 ? -1 : 0;
}

float engine::VoxelRaycast::mod(int position, int modulus) {
    float local = position % modulus;
    if (local < 0) {
        local += modulus;
    }
    return local;
}

float engine::VoxelRaycast::intbound(float s, float ds) {
    if (ds < 0) {
        return intbound(-s, -ds);
    } else {
        s = mod(s, 1);
        return (1-s)/ds;
    }
}
