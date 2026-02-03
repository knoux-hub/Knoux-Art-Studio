import React from 'react';
import { AppScreen, UserMode } from '../types';
import BodyEditorView from '../../views/BodyEditor';

interface Props {
  navigate: (s: AppScreen) => void;
  userMode: UserMode;
}

const BodyEditor: React.FC<Props> = ({ navigate, userMode }) => (
  <BodyEditorView navigate={navigate} userMode={userMode} />
);

export default BodyEditor;
