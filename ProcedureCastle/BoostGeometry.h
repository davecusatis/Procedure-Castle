#pragma once
#include <boost\geometry.hpp>
#include <boost\geometry\geometries\point_xy.hpp>
#include <boost\geometry\geometries\polygon.hpp>
#include <boost\geometry\geometries\box.hpp>
#include <boost\geometry\geometries\linestring.hpp>
#include <vector>

typedef boost::geometry::model::d2::point_xy<double> Point2;
typedef boost::geometry::model::polygon<Point2> Polygon2;
typedef boost::geometry::model::linestring<Point2> Polyline2;
typedef boost::geometry::model::box<Point2> Box2;
typedef std::vector<Polygon2> Polygon2Container;

