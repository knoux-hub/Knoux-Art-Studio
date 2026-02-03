import React from 'react';

const TemplateGallery: React.FC = () => {
  const templates = Array.from({ length: 12 }).map((_, index) => ({
    id: `template-${index + 1}`,
    name: `Template ${index + 1}`,
  }));

  return (
    <div className="template-gallery">
      <h2>📚 Templates</h2>
      <div className="template-grid">
        {templates.map((template) => (
          <div key={template.id} className="template-card">
            <div className="template-preview" />
            <div className="template-meta">
              <span>{template.name}</span>
              <button>Select</button>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
};

export default TemplateGallery;
