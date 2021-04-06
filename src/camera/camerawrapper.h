#ifndef MELON_CAMERAWRAPPER_H
#define MELON_CAMERAWRAPPER_H

#include "abstractcamera.h"
#include <memory>

/** @brief Wrapper for initializing cameras
 *
 * This is a wrapper class for AbstractCamera and derivative classes. Internally it holds a smart pointer to an
 * AbstractCamera. It's mainly for easily managing the multiple possible camera types
 *
 */
class CameraWrapper : public UpdateableState
{
public:
     /** @brief Create a new CameraWrapper instance
     *
     * @param variables [in] Current program state
     * @throws std::runtime_error If camera type within current state is invalid
     */
    explicit CameraWrapper(StateVariables& variables);

    // Gets the current camera instance
    // NOTE: DO NOT STORE THIS!! If this pointer is stored somewhere and the internal camera pointer changes,
    // this pointer will be invalidated!
    // NOTE: NOT THREAD SAFE
    /** @brief Gets the current camera instance
     *
     * This performs a dereference to the internal AbstractCamera smart pointer, allowing functions and class members
     * of AbstractCamera to "passthrough" without having getter function calls everywhere
     *
     * @note DO NOT STORE THE RETURNED POINTER. If this pointer is stored somewhere and the internal camera pointer
     * changes (like in the event that the camera type changes) then the stored pointer will be invalidated
     *
     * @return Pointer to the internal AbstractCamera instance
     */
    AbstractCamera* operator->() const noexcept;

    // Calls the wrapped camera's update_state implementation
    /** @brief Update internal camera with current program state
     *
     * If the camera type in the state has changed, a new camera instance will be created.
     * Otherwise AbstractCamera::update_state() is called for the internal camera
     *
     * @param state
     */
    void update_state(const StateVariables& state) override;

private:
    std::unique_ptr<AbstractCamera> m_camera;

    /** @brief Create a new AbstractCamera instance
     *
     * This creates a new instance of an AbstractCamera derivative based on the current program state and casts it
     * back into an AbstractCamera using a smart pointer
     *
     * @param state Current program state
     * @return Smart pointer to AbstractCamera instance
     * @throws std::runtime_error if camera type in state is invalid
     */
    static std::unique_ptr<AbstractCamera> new_camera(const StateVariables& state);
};


#endif //MELON_CAMERAWRAPPER_H
