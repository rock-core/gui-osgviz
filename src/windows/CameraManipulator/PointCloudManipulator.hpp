#ifndef OSGVIZ_OSGVIZ_SRC_WINDOWS_CAMERAMANIPULATOR_POINTCLOUDMANIPULATOR_H_
#define OSGVIZ_OSGVIZ_SRC_WINDOWS_CAMERAMANIPULATOR_POINTCLOUDMANIPULATOR_H_

#include <osgGA/TerrainManipulator>

namespace osgviz
{
    /** A TerrainManipulator that also snaps its rotation center onto point clouds
     *
     * osgGA::TerrainManipulator places its center (the point it rotates
     * around and zooms towards) by intersecting the scene with a
     * LineSegmentIntersector. That intersector only reports triangles, so
     * point clouds are never hit and the center ends up at a wrong height
     * (e.g. on a ground plane below the cloud).
     *
     * This manipulator additionally casts a thin box (a "wide ray") around the
     * same segments using a PolytopeIntersector restricted to points, and
     * uses the hit closest to the start of the segment. The half-width of the
     * box is getPickTolerance() times the current camera-to-center distance.
     *
     * TerrainManipulator::intersect is not virtual, so all methods that use
     * it (setByMatrix, setTransformation and panning) are reimplemented here.
     */
    class PointCloudManipulator : public osgGA::TerrainManipulator
    {
    public:
        PointCloudManipulator(int flags = DEFAULT_SETTINGS);

        virtual void setByMatrix(const osg::Matrixd& matrix) override;
        virtual void setTransformation(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up) override;

        /** Half-width of the ray used to find points, relative to the
         * camera-to-center distance (default: 0.01) */
        void setPickTolerance(double tolerance);
        double getPickTolerance() const;

    protected:
        virtual bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy) override;

        /** Intersects the segment with triangles and points of the scene
         *
         * On success, \c intersection is the closest hit projected onto the
         * segment, so that using it as the center does not move the view
         * sideways.
         */
        bool intersectScene(const osg::Vec3d& start, const osg::Vec3d& end, osg::Vec3d& intersection) const;

    private:
        double pickTolerance;
    };
}

#endif
