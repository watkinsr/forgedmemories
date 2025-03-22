#include "Algorithm.h"
#include "Log.h"

ssize_t binary_search_exact(std::vector<Placement>* placements, Vector2D* v, int l, int r) {
    if (r==l) return -1;
    int idx = (r-l)/2;
    Placement* comparator = &(placements)->at(idx);
    LOG(1, "INFO", "Match? <Placement [%i, %i]> against [%i,%i]\n", comparator->x, comparator->y, v->x, v->y);
    if (comparator->y == v->y) {
        if (comparator->x == v->x)  return idx;
        else                        return binary_search_exact(placements, v, l, idx);
    }
    else if (comparator->y > v->y)  return binary_search_exact(placements, v, l, idx);
    else                            return binary_search_exact(placements, v, idx, r);
}

// placements: Pre-sorted by Y and then X.
// vl: Top left of region.
// vr: Bottom right of region.
// l : Sliding window left index
// r : Sliding window right index
ssize_t binary_search_region(std::vector<Placement>* placements, Vector2D* vl, Vector2D* vr, int l, int r) {
    if (l>r) return -1;
    int mid = l+(r-l)/2;
    Placement* comparator = &(placements)->at(mid);

    LOG(1, "INFO", "Sliding Window [%i, %i]\n", l, r);
    LOG(1, "INFO", "[%i] Match Region? <Placement [%i, %i]> against [tl=[%i,%i], tr=[%i,%i]]\n", mid, comparator->x, comparator->y, vl->x, vl->y, vr->x, vr->y);

    if (comparator->y < vl->y)                          return binary_search_region(placements, vl, vr, mid+1, r);  // Search right.
    if (comparator->y > vl->y && comparator->y > vr->y) return binary_search_region(placements, vl, vr, l, mid-1);  // Search left.

    if (comparator->y >= vl->y && comparator->y <= vr->y && comparator->x >= vl->x && comparator->x <= vr->x) {
        return mid;  // Exact match.
    }

    if (comparator->y >= vl->y && comparator->y <= vr->y) {
        // We're in the row ballpark.
        if (comparator->x < vl->x) return binary_search_region(placements, vl, vr, mid+1, r);  // Search right.
        else                       return binary_search_region(placements, vl, vr, l, mid-1);  // Search left.
    }
}
