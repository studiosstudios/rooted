//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_CARROT_H
#define ROOTED_CARROT_H

#include "EntityModel.h"

using namespace cugl;

class Carrot : public EntityModel {
    
private:
    /** if this carrot has been captured */
    bool _isCaptured;
    /** if this carrot is rooted*/
    bool _isRooted;
    /** number of baby carrots this carrot has caught so far */
    int _numBabyCarrots;
        
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a degenerate Carrot.
     *
     * This constructor does not initialize any of the carrot values beyond
     * the defaults.  To use a Carrot, you must call init().
     */
    Carrot() {};

    /**
     * Destroys this Carrot, releasing all resources. Currently overrides
     * destructor in EntityModel, but does the same thing.
     */
    ~Carrot() override { dispose(); };

    /**
     * Initializes a new Carrot at the given position.
     *
     * The Carrot is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the Carrot in world units
     * @param scale The drawing scale (world to screen)
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale) override;

    /**
     * Creates a new Carrot at the given position.
     *
     * The Carrot is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     * @param scale The drawing scale (world to screen)
     *
     * @return  A newly allocated Carrot at the given position with the given scale
     */
    static std::shared_ptr<Carrot> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Carrot> result = std::make_shared<Carrot>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

#pragma mark -
#pragma mark Getters and Setters
    bool isRooted() { return _isRooted; };
    
    bool isCaptured() {return _isCaptured; };
    
    int getNumBabyCarrots() { return  _numBabyCarrots; };
    
#pragma mark -
#pragma mark Interactions
    void captureBabyCarrot() { _numBabyCarrots++; };
    
    void gotCaptured();
    
    void gotRooted();
    
    void gotUnrooted();
};


#endif //ROOTED_CARROT_H
