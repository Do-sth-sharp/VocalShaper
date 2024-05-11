#pragma once

#include <JuceHeader.h>

class KeyMappingEditorComponent : public juce::Component {
public:
    //==============================================================================
    /** Creates a KeyMappingEditorComponent.

        @param mappingSet   this is the set of mappings to display and edit. Make sure the
                            mappings object is not deleted before this component!
        @param showResetToDefaultButton     if true, then at the bottom of the list, the
                                            component will include a 'reset to defaults' button.
    */
    KeyMappingEditorComponent(juce::KeyPressMappingSet& mappingSet,
        bool showResetToDefaultButton);

    /** Destructor. */
    ~KeyMappingEditorComponent() override;

    //==============================================================================
    /** Sets up the colours to use for parts of the component.

        @param mainBackground       colour to use for most of the background
        @param textColour           colour to use for the text
    */
    void setColours(juce::Colour mainBackground,
        juce::Colour textColour);

    /** Returns the KeyPressMappingSet that this component is acting upon. */
    juce::KeyPressMappingSet& getMappings() const noexcept { return this->mappings; }

    /** Returns the ApplicationCommandManager that this component is connected to. */
    juce::ApplicationCommandManager& getCommandManager() const noexcept { return this->mappings.getCommandManager(); }


    //==============================================================================
    /** Can be overridden if some commands need to be excluded from the list.

        By default this will use the KeyPressMappingSet's shouldCommandBeVisibleInEditor()
        method to decide what to return, but you can override it to handle special cases.
    */
    virtual bool shouldCommandBeIncluded(juce::CommandID commandID);

    /** Can be overridden to indicate that some commands are shown as read-only.

        By default this will use the KeyPressMappingSet's shouldCommandBeReadOnlyInEditor()
        method to decide what to return, but you can override it to handle special cases.
    */
    virtual bool isCommandReadOnly(juce::CommandID commandID);

    /** This can be overridden to let you change the format of the string used
        to describe a keypress.

        This is handy if you're using non-standard KeyPress objects, e.g. for custom
        keys that are triggered by something else externally. If you override the
        method, be sure to let the base class's method handle keys you're not
        interested in.
    */
    virtual juce::String getDescriptionForKeyPress(const juce::KeyPress& key);

    //==============================================================================
    /** A set of colour IDs to use to change the colour of various aspects of the editor.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        backgroundColourId = 0x100ad00,    /**< The background colour to fill the editor background. */
        textColourId = 0x100ad01,    /**< The colour for the text. */
    };

    //==============================================================================
    /** @internal */
    void parentHierarchyChanged() override;
    /** @internal */
    void resized() override;

private:
    //==============================================================================
    juce::KeyPressMappingSet& mappings;
    juce::TreeView tree;
    juce::TextButton resetButton;

    class TopLevelItem;
    class ChangeKeyButton;
    class MappingItem;
    class CategoryItem;
    class ItemComponent;
    std::unique_ptr<TopLevelItem> treeItem;
    juce::ScopedMessageBox messageBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyMappingEditorComponent)
};
