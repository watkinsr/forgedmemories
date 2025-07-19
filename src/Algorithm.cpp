#include "Algorithm.h"
#include "Log.h"
#include "Macros.h"

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
    LOG(1, "TRACE", "binary_search_region(<Placements size=%u>, vl=[%u,%u], vr=[%u,%u], l=%i, r=%i)\n", placements->size(), vl->x, vl->y, vr->x, vr->y, l, r);
    if (l>r) return -1;
    int mid = l+(r-l)/2;
    Placement* comparator = &(placements)->at(mid);

    LOG(1, "INFO", "Sliding Window [%i, %i]\n", l, r);
    LOG(1, "INFO", "[mid=%i] <Placement [%i, %i]> MATCH? [top-left=[%i,%i], bottom-right=[%i,%i]]\n", mid, comparator->x, comparator->y, vl->x, vl->y, vr->x, vr->y);

    if (comparator->y < vl->y)                          return binary_search_region(placements, vl, vr, mid+1, r);  // Search right.
    if (comparator->y > vl->y && comparator->y > vr->y) return binary_search_region(placements, vl, vr, l, mid-1);  // Search left.

    // [mid=3] <Placement [288, 160]> MATCH? [top-left=[288,160], bottom-right=[288,128]]
    // FIXME: top-left and bottom-right here MAKE NO SENSE.
    if (comparator->y >= vl->y && comparator->y <= vr->y && comparator->x >= vl->x && comparator->x <= vr->x) {
        return mid;  // Exact match.
    }

    if (comparator->y >= vl->y && comparator->y <= vr->y) {
        // We're in the row ballpark.
        if (comparator->x < vl->x) return binary_search_region(placements, vl, vr, mid+1, r);  // Search right.
        else                       return binary_search_region(placements, vl, vr, l, mid-1);  // Search left.
    }

    LOG(1, "PANIC", "FIXME: NoMatchPanic - This is very likely a bug but I'm super tired\n");
    // ASSERT_NOT_REACHED();
    return -1;

}
