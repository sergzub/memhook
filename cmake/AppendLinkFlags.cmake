function(append_link_flags _TARGET _FLAGS)
  set(NEW_LINK_FLAGS ${_FLAGS})
  get_target_property(EXISTING_LINK_FLAGS ${_TARGET} LINK_FLAGS)
  if(EXISTING_LINK_FLAGS)
    set(NEW_LINK_FLAGS "${EXISTING_LINK_FLAGS} ${NEW_LINK_FLAGS}")
  endif()
  set_target_properties(${_TARGET} PROPERTIES LINK_FLAGS ${NEW_LINK_FLAGS})
endfunction()
