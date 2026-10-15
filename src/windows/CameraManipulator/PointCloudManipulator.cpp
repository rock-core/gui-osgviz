#include "PointCloudManipulator.hpp"

#include <osg/Polytope>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <algorithm>
#include <limits>

using namespace osgviz;

PointCloudManipulator::PointCloudManipulator(int flags)
    : osgGA::TerrainManipulator(flags)
    , pickTolerance(0.01)
{
}

void PointCloudManipulator::setPickTolerance(double tolerance)
{
    pickTolerance = tolerance;
}

double PointCloudManipulator::getPickTolerance() const
{
    return pickTolerance;
}

bool PointCloudManipulator::intersectScene(const osg::Vec3d& start, const osg::Vec3d& end, osg::Vec3d& intersection) const
{
    if (!_node.valid())
        return false;

    osg::Vec3d direction = end - start;
    const double length = direction.normalize();
    if (length <= 0)
        return false;

    double bestT = std::numeric_limits<double>::infinity();

    // triangles (meshes, ground planes, ...): the default terrain intersection
    osg::Vec3d triangleHit;
    if (intersect(start, end, triangleHit))
        bestT = (triangleHit - start) * direction;

    // points: a thin box around the segment, since a line never hits a point
    osg::Vec3d u = direction ^ osg::Vec3d(0, 0, 1);
    if (u.length2() < 1e-6)
        u = direction ^ osg::Vec3d(1, 0, 0);
    u.normalize();
    osg::Vec3d v = direction ^ u;
    v.normalize();
    const double radius = std::max(pickTolerance * _distance, 1e-6);

    osg::Polytope box;
    box.add(osg::Plane(u, start - u * radius));
    box.add(osg::Plane(-u, start + u * radius));
    box.add(osg::Plane(v, start - v * radius));
    box.add(osg::Plane(-v, start + v * radius));
    box.add(osg::Plane(direction, start));
    box.add(osg::Plane(-direction, end));

    osg::ref_ptr<osgUtil::PolytopeIntersector> pi = new osgUtil::PolytopeIntersector(box);
    pi->setPrimitiveMask(osgUtil::PolytopeIntersector::POINT_PRIMITIVES);
    osgUtil::IntersectionVisitor iv(pi.get());
    iv.setTraversalMask(_intersectTraversalMask);
    _node->accept(iv);

    for (const osgUtil::PolytopeIntersector::Intersection& hit : pi->getIntersections())
    {
        for (unsigned int i = 0; i < hit.numIntersectionPoints; ++i)
        {
            osg::Vec3d point = hit.intersectionPoints[i];
            if (hit.matrix.valid())
                point = point * (*hit.matrix);
            const double t = (point - start) * direction;
            if (t >= 0 && t < bestT)
                bestT = t;
        }
    }

    if (bestT > length)
        return false;
    intersection = start + direction * bestT;
    return true;
}

void PointCloudManipulator::setByMatrix(const osg::Matrixd& matrix)
{
    // keep the current distance as fallback in case nothing is hit
    osgGA::OrbitManipulator::setByMatrix(matrix);

    if (_node.valid())
    {
        const osg::Vec3d eye = matrix.getTrans();
        const osg::Vec3d look = _rotation * osg::Vec3d(0, 0, -1);
        const osg::BoundingSphere& bs = _node->getBound();
        const double range = (eye - bs.center()).length() + bs.radius();

        osg::Vec3d ip;
        if (intersectScene(eye, eye + look * range, ip))
        {
            _center = ip;
            _distance = (ip - eye).length();
        }
    }

    _previousUp = getUpVector(getCoordinateFrame(_center));
    clampOrientation();
}

void PointCloudManipulator::setTransformation(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up)
{
    osgGA::OrbitManipulator::setTransformation(eye, center, up);

    if (_node.valid())
    {
        const osg::BoundingSphere& bs = _node->getBound();
        const osg::Vec3d look = _rotation * osg::Vec3d(0, 0, -1);
        const double range = _distance + 2 * (eye - bs.center()).length() + bs.radius();

        osg::Vec3d ip;
        if (intersectScene(eye, eye + look * range, ip))
        {
            _center = ip;
            _distance = (ip - eye).length();
        }
    }

    _previousUp = getUpVector(getCoordinateFrame(_center));
    clampOrientation();
}

bool PointCloudManipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy)
{
    // pan within the plane perpendicular to the local up vector (as TerrainManipulator)
    const double scale = -0.3 * _distance * getThrowScale(eventTimeDelta);
    const osg::Vec3d localUp = _previousUp;
    osg::Vec3d sideVector = getSideVector(osg::Matrixd(_rotation));
    osg::Vec3d forwardVector = localUp ^ sideVector;
    sideVector = forwardVector ^ localUp;
    forwardVector.normalize();
    sideVector.normalize();
    _center += forwardVector * (dy * scale) + sideVector * (dx * scale);

    if (!_node.valid())
        return true;

    // move the center up or down onto the closest surface or point
    const osg::Vec3d up = getUpVector(getCoordinateFrame(_center));
    const double range = _node->getBound().radius() * 0.25;
    osg::Vec3d above, below;
    const bool hitAbove = intersectScene(_center, _center + up * range, above);
    const bool hitBelow = intersectScene(_center, _center - up * range, below);
    if (hitAbove && hitBelow)
        _center = ((above - _center).length2() < (below - _center).length2()) ? above : below;
    else if (hitAbove)
        _center = above;
    else if (hitBelow)
        _center = below;

    // keep the view consistent if the up vector changed (e.g. on a globe)
    const osg::Vec3d newUp = getUpVector(getCoordinateFrame(_center));
    osg::Quat panRotation;
    panRotation.makeRotate(localUp, newUp);
    if (!panRotation.zeroRotation())
    {
        _rotation = _rotation * panRotation;
        _previousUp = newUp;
    }
    return true;
}
