#include "Product.h"

Product::Product()
    : name(""),
      category(""),
      baseCost(0),
      todayCost(0),
      suggestedPrice(0),
      playerPrice(0),
      stock(0),
      maxStock(0),
      demandLevel(0),
      priceSensitivity(0),
      eventSensitivity(0),
      fluctuationSensitivity(0),
      unlocked(false)
{
}
